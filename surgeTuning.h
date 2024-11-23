#pragma once
// this is a port of Tunings.h by Paul Walker to the HexBoard.
// -*-c++-*-
/**
 * Tunings.h
 * Copyright Paul Walker, 2019-2020
 * Released under the MIT License. See LICENSE.md
 *
 * Tunings.h contains the public API required to determine full keyboard frequency maps
 * for a scala SCL and KBM file in standalone, tested, open licensed C++ header only library.
 *
 * An example of using the API is
 *
 * ```
 * auto s = Tunings::readSCLFile( "./my-scale.scl" );
 * auto k = Tunings::readKBMFile( "./my-mapping.kbm" );
 *
 * Tunings::Tuning t( s, k );
 *
 * std::cout << "The frequency of C4 and A4 are "
 *           << t.frequencyForMidiNote( 60 ) << " and "
 *           << t.frequencyForMidiNote( 69 ) << std::endl;
 * ```
 *
 * The API provides several other points, such as access to the structure of the SCL and KBM,
 * the ability to create several prototype SCL and KBM files wthout SCL or KBM content,
 * a frequency measure which is normalized by the frequency of standard tuning midi note 0
 * and the logarithmic frequency scale, with a doubling per frequency doubling.
 *
 * Documentation is in the class header below; tests are in `tests/all_tests.cpp` and
 * a variety of command line tools accompany the header.
 * TuningsImpl.h
 * Copyright 2019-2020 Paul Walker
 * Released under the MIT License. See LICENSE.md
 *
 * This contains the nasty nitty gritty implementation of the api in Tunings.h. You probably
 * don't need to read it unless you have found and are fixing a bug, are curious, or want
 * to add a feature to the API. For usages of this library, the documentation in Tunings.h and
 * the usages in tests/all_tests.cpp should provide you more than enough guidance.
 */
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <array>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <math.h>
#include <sstream>
#include <cctype>
#include <cmath>

namespace Tunings {
	static constexpr double MIDI_0_FREQ = 8.17579891564371; // or 440.0 * pow( 2.0, - (69.0/12.0 ) )
	/**
	 * A Tone is a single entry in an SCL file. It is expressed either in cents or in
	 * a ratio, as described in the SCL documentation.
	 *
	 * In most normal use, you will not use this class, and it will be internal to a Scale
	 */
	struct Tone {
		typedef enum Type {
			kToneCents, // An SCL representation like "133.0"
			kToneRatio  // An SCL representation like "3/7"
		} Type;
		Type type;
		double cents;
		int64_t ratio_d, ratio_n;
		std::string stringRep;
		double floatValue; // cents / 1200 + 1.
		int lineno; // which line of the SCL does this tone appear on?
		Tone() : type(kToneRatio), cents(0), ratio_d(1), ratio_n(1), stringRep("1/1"), floatValue(1.0) {}
	};

	// Thank you to: https://gist.github.com/josephwb/df09e3a71679461fc104
	inline std::istream& getlineEndingIndependent(std::istream &is, std::string &t) {
		t.clear();
		std::istream::sentry se(is, true);
		if (!se)
			return is;
		std::streambuf *sb = is.rdbuf();
		for (;;) {
			int c = sb->sbumpc();
			switch (c) {
				case '\n':
					return is;
				case '\r':
					if (sb->sgetc() == '\n') {
						sb->sbumpc();
					}
					return is;
				case EOF:
					is.setstate(std::ios::eofbit);
					if (t.empty()) {
						is.setstate(std::ios::badbit);
					}
					return is;
				default:
					t += (char)c;
				}
		}
	}

	inline double locale_atof(const char *s) {
		double result = 0;
		std::istringstream istr(s);
		istr.imbue(std::locale("C"));
		istr >> result;
		return result;
	}

	/**
	 * Given an SCL string like "100.231" or "3/7" set up a Tone
	 */
	inline Tone toneFromString(const std::string &line, int lineno) {
		Tone t;
		t.stringRep = line;
		t.lineno = lineno;
		if (line.find('.') != std::string::npos) {
			t.type = Tone::kToneCents;
			t.cents = locale_atof(line.c_str());
		}	else {
			t.type = Tone::kToneRatio;
			auto slashPos = line.find('/');
			if (slashPos == std::string::npos) {
				t.ratio_n = atoll(line.c_str());
				t.ratio_d = 1;
			}	else {
				t.ratio_n = atoll(line.substr(0, slashPos).c_str());
				t.ratio_d = atoll(line.substr(slashPos + 1).c_str());
			}
			if (t.ratio_n == 0 || t.ratio_d == 0)	{
				std::string s = "Invalid tone in SCL file.";
				if (lineno >= 0)
					s += "Line " + std::to_string(lineno) + ".";
				s += " Line is '" + line + "'.";
				throw TuningError(s);
			}
			// 2^(cents/1200) = n/d
			// cents = 1200 * log(n/d) / log(2)
			t.cents = 1200 * log(1.0 * t.ratio_n / t.ratio_d) / log(2.0);
		}
		t.floatValue = t.cents / 1200.0 + 1.0;
		return t;
	}

	/**
	 * The Scale is the representation of the SCL file. It contains several key
	 * features. Most importantly it has a count and a vector of Tones.
	 *
	 * In most normal use, you will simply pass around instances of this class
	 * to a Tunings::Tuning instance, but in some cases you may want to create
	 * or inspect this class yourself. Especially if you are displaying this
	 * class to your end users, you may want to use the rawText or count methods.
	 */
	struct Scale {
		std::string name;        // The name in the SCL file. Informational only
		std::string description; // The description in the SCL file. Informational only
		std::string rawText;     // The raw text of the SCL file used to create this Scale
		int count;               // The number of tones.
		std::vector<Tone> tones; // The tones

		Scale() : name("empty scale"), description(""), rawText(""), count(0) {}
	};

	/**
	 * In some failure states, the tuning library will throw an exception of
	 * type TuningError with a descriptive message.
	 */
	class TuningError : public std::exception	{
		public:
			TuningError(std::string m) : whatv(m) {}
			virtual const char *what() const noexcept override { return whatv.c_str(); }
		private:
			std::string whatv;
	};

	/**
	 * readSCLStream returns a Scale from the SCL input stream
	 */
	inline Scale readSCLStream(std::istream &inf)	{
		std::string line;
		const int read_header = 0, read_count = 1, read_note = 2, trailing = 3;
		int state = read_header;
		Scale res;
		std::ostringstream rawOSS;
		int lineno = 0;
		while (getlineEndingIndependent(inf, line)) {
			rawOSS << line << "\n";
			lineno++;
			if ((state == read_note && line.empty()) || line[0] == '!') {
				continue;
			}
			switch (state) {
				case read_header:
					res.description = line;
					state = read_count;
					break;
				case read_count:
					res.count = atoi(line.c_str());
					if (res.count < 1) {
						throw TuningError("Invalid SCL note count.");
					}
					state = read_note;
					break;
				case read_note:
					auto t = toneFromString(line, lineno);
					res.tones.push_back(t);
					if ((int)res.tones.size() == res.count)
						state = trailing;
					break;
			}
		}
		if (!(state == read_note || state == trailing)) {
			std::ostringstream oss;
			oss << "Incomplete SCL content. Only able to read " << lineno
					<< " lines of data. Found content up to ";
			switch (state) {
				case read_header:
					oss << "reading header.";
					break;
				case read_count:
					oss << "reading scale count.";
					break;
				default:
					oss << "unknown state.";
					break;
			}
			throw TuningError(oss.str());
		}
		if ((int)res.tones.size() != res.count) {
			std::string s =
				"Read fewer notes than count in file. Count = " + std::to_string(res.count) +
				" notes. Array size = " + std::to_string(res.tones.size());
			throw TuningError(s);
		}
		res.rawText = rawOSS.str();
		return res;
	}

	/**
	 * readSCLFile returns a Scale from the SCL File in fname
	 */
	inline Scale readSCLFile(std::string fname)	{
		std::ifstream inf;
		inf.open(fname);
		if (!inf.is_open())	{
			std::string s = "Unable to open file '" + fname + "'";
			throw TuningError(s);
		}
		auto res = readSCLStream(inf);
		res.name = fname;
		return res;
	}

	/**
	 * parseSCLData returns a scale from the SCL file contents in memory
	 */
	inline Scale parseSCLData(const std::string &d) {
		std::istringstream iss(d);
		auto res = readSCLStream(iss);
		res.name = "Scale from patch";
		return res;
	}

	/**
	 * evenTemperament12NoteScale provides a utility scale which is
	 * the "standard tuning" scale
	 */
	inline Scale evenTemperament12NoteScale() {
		std::string data = R"SCL(! 12 Tone Equal Temperament.scl
			!
			12 Tone Equal Temperament | ED2-12 - Equal division of harmonic 2 into 12 parts
			12
			!
			100.00000
			200.00000
			300.00000
			400.00000
			500.00000
			600.00000
			700.00000
			800.00000
			900.00000
			1000.00000
			1100.00000
			2/1
			)SCL";
		return parseSCLData(data);
	}

	/**
	 * evenDivisionOfSpanByM provides a scale referd to as "ED2-17" or
	 * "ED3-24" by dividing the Span into M points. eventDivisionOfSpanByM(2,12)
	 * should be the evenTemperament12NoteScale
	 */
	inline Scale evenDivisionOfSpanByM(int Span, int M) {
		if (Span <= 0)
			throw Tunings::TuningError(
				"Span should be a positive number. You entered " + std::to_string(Span)
			);
		if (M <= 0)
			throw Tunings::TuningError(
				"You must divide the period into at least one step. You entered " + std::to_string(M)
			);
		std::ostringstream oss;
		oss.imbue(std::locale("C"));
		oss << "! Automatically generated ED" << Span << "-" << M << " scale\n";
		oss << "Automatically generated ED" << Span << "-" << M << " scale\n";
		oss << M << "\n";
		oss << "!\n";
		double topCents = 1200.0 * log(1.0 * Span) / log(2.0);
		double dCents = topCents / M;
		for (int i = 1; i < M; ++i)
			oss << std::fixed << dCents * i << "\n";
		oss << Span << "/1\n";
		return parseSCLData(oss.str());
	}

	/**
	 * evenDivisionOfCentsByM provides a scale which divides Cents into M
	 * steps. It is less frequently used than evenDivisionOfSpanByM for obvious
	 * reasons. If you want the last cents label labeled differently than the cents
	 * argument, pass in the associated optional label
	 */
	inline Scale evenDivisionOfCentsByM(float Cents, int M, const std::string &lastLabel) {
		if (Cents <= 0)
			throw Tunings::TuningError(
				"Cents should be a positive number. You entered " + std::to_string(Cents)
			);
		if (M <= 0)
			throw Tunings::TuningError(
				"You must divide the period into at least one step. You entered " + std::to_string(M)
			);
		std::ostringstream oss;
		oss.imbue(std::locale("C"));
		oss << "! Automatically generated Even Division of " << Cents << " ct into " << M << " scale\n";
		oss << "Automatically generated Even Division of " << Cents << " ct into " << M << " scale\n";
		oss << M << "\n";
		oss << "!\n";
		double topCents = Cents;
		double dCents = topCents / M;
		for (int i = 1; i < M; ++i)
			oss << std::fixed << dCents * i << "\n";
		if (lastLabel.empty())
			oss << Cents << "\n";
		else
			oss << lastLabel << "\n";
		return parseSCLData(oss.str());
	}

	/**
	 * The KeyboardMapping class represents a KBM file. In most cases, the salient
	 * features are the tuningConstantNote and tuningFrequency, which allow you to
	 * pick a fixed note in the midi keyboard when retuning. The KBM file can also
	 * remap individual keys to individual points in a scale, which kere is done with the
	 * keys vector.
	 *
	 * Just as with Scale, the rawText member contains the text of the KBM file used.
	 */
	struct KeyboardMapping {
		int count;
		int firstMidi, lastMidi;
		int middleNote;
		int tuningConstantNote;
		double tuningFrequency, tuningPitch; // pitch = frequency / MIDI_0_FREQ
		int octaveDegrees;
		std::vector<int> keys; // rather than an 'x' we use a '-1' for skipped keys

		std::string rawText;
		std::string name;

		inline KeyboardMapping()
		: count(0)
		, firstMidi(0)
		, lastMidi(127)
		, middleNote(60)
		, tuningConstantNote(60)
		,	tuningFrequency(MIDI_0_FREQ * 32.0)
		, tuningPitch(32.0)
		, octaveDegrees(0)
		, rawText("")
		,	name("") {
			std::ostringstream oss;
			oss.imbue(std::locale("C"));
			oss << "! Default KBM file\n";
			oss << count << "\n"
					<< firstMidi << "\n"
					<< lastMidi << "\n"
					<< middleNote << "\n"
					<< tuningConstantNote << "\n"
					<< tuningFrequency << "\n"
					<< octaveDegrees << "\n";
			rawText = oss.str();
		}
	};

	/**
	 * readKBMStream returns a KeyboardMapping from a KBM input stream
	 */
	inline KeyboardMapping readKBMStream(std::istream &inf) {
		std::string line;
		KeyboardMapping res;
		std::ostringstream rawOSS;
		res.keys.clear();
		enum parsePosition {
				map_size = 0,
				first_midi,
				last_midi,
				middle,
				reference,
				freq,
				degree,
				keys,
				trailing
		};
		parsePosition state = map_size;
		int lineno = 0;
		while (getlineEndingIndependent(inf, line)) {
			rawOSS << line << "\n";
			lineno++;
			if (line[0] == '!') {
				continue;
			}
			if (line == "x")
					line = "-1";
			else if (state != trailing) {
				const char *lc = line.c_str();
				bool validLine = line.length() > 0;
				char badChar = '\0';
				while (validLine && *lc != '\0') {
					if (!(
						   *lc == ' ' || std::isdigit(*lc) 
						|| *lc == '.' || *lc == (char)13 
						|| *lc == '\n')
					) {
						validLine = false;
						badChar = *lc;
					}
					lc++;
				}
				if (!validLine) {
					throw TuningError(
						  "Invalid line " + std::to_string(lineno) + ". line='" + line 
						+ "'. Bad character is '" + badChar + "/" + std::to_string((int)badChar) + "'"
					);
				}
			}
			int i = std::atoi(line.c_str());
			double v = locale_atof(line.c_str());
			switch (state) {
				case map_size:
					res.count = i;
					break;
				case first_midi:
					res.firstMidi = i;
					break;
				case last_midi:
					res.lastMidi = i;
					break;
				case middle:
					res.middleNote = i;
					break;
				case reference:
					res.tuningConstantNote = i;
					break;
				case freq:
					res.tuningFrequency = v;
					res.tuningPitch = res.tuningFrequency / 8.17579891564371;
					break;
				case degree:
					res.octaveDegrees = i;
					break;
				case keys:
					res.keys.push_back(i);
					if ((int)res.keys.size() == res.count)
							state = trailing;
					break;
				case trailing:
					break;
			}
			if (!(state == keys || state == trailing))
				state = (parsePosition)(state + 1);
			if (state == keys && res.count == 0)
				state = trailing;
		}
		if (!(state == keys || state == trailing)) {
			std::ostringstream oss;
			oss << "Incomplete KBM stream. Only able to read " << lineno << " lines. Read up to ";
			switch (state) {
				case map_size:
					oss << "map size.";
					break;
				case first_midi:
					oss << "first midi note.";
					break;
				case last_midi:
					oss << "last midi note.";
					break;
				case middle:
					oss << "scale zero note.";
					break;
				case reference:
					oss << "scale reference note.";
					break;
				case freq:
					oss << "scale reference frequency.";
					break;
				case degree:
					oss << "scale degree.";
					break;
				default:
					oss << "unknown state";
					break;
			}
			throw TuningError(oss.str());
		}
		if ((int)res.keys.size() != res.count) {
			throw TuningError(
				"Different number of keys than mapping file indicates. Count is " 
				+	std::to_string(res.count) + " and we parsed " 
				+ std::to_string(res.keys.size()) + " keys."
			);
		}
		res.rawText = rawOSS.str();
		return res;
	}

	/**
	 * readKBMFile returns a KeyboardMapping from a KBM file name
	 */
	inline KeyboardMapping readKBMFile(std::string fname) {
		std::ifstream inf;
		inf.open(fname);
		if (!inf.is_open())	{
			std::string s = "Unable to open file '" + fname + "'";
			throw TuningError(s);
		}
		auto res = readKBMStream(inf);
		res.name = fname;
		return res;
	}

	/**
	 * parseKBMData returns a KeyboardMapping from a KBM data in memory
	 */
	inline KeyboardMapping parseKBMData(const std::string &d)	{
		std::istringstream iss(d);
		auto res = readKBMStream(iss);
		res.name = "Mapping from patch";
		return res;
	}

	/**
	 * startScaleOnAndTuneNoteTo generates a KBM where scaleStart is the note 0
	 * of the scale, where midiNote is the tuned note, and where feq is the frequency
	 */
	inline KeyboardMapping startScaleOnAndTuneNoteTo(int scaleStart, int midiNote, double freq) {
		std::ostringstream oss;
		oss.imbue(std::locale("C"));
		oss << "! Automatically generated mapping, tuning note " << midiNote << " to " << freq
				<< " Hz\n"
				<< "!\n"
				<< "! Size of map\n"
				<< 0 << "\n"
				<< "! First and last MIDI notes to map - map the entire keyboard\n"
				<< 0 << "\n"
				<< 127 << "\n"
				<< "! Middle note where the first entry in the scale is mapped.\n"
				<< scaleStart << "\n"
				<< "! Reference note where frequency is fixed\n"
				<< midiNote << "\n"
				<< "! Frequency for MIDI note " << midiNote << "\n"
				<< freq << "\n"
				<< "! Scale degree for formal octave. This is an empty mapping, so:\n"
				<< 0 << "\n"
				<< "! Mapping. This is an empty mapping so list no keys\n";
		return parseKBMData(oss.str());
	}

	/**
	 * tuneNoteTo creates a KeyboardMapping which keeps the midi note given is set
	 * to a constant frequency, given
	 */
	inline KeyboardMapping tuneNoteTo(int midiNote, double freq) {
		return startScaleOnAndTuneNoteTo(60, midiNote, freq);
	}
	/**
	 * tuneA69To creates a KeyboardMapping which keeps the midi note 69 (A4) set
	 * to a constant frequency, given
	 */
	inline KeyboardMapping tuneA69To(double freq) { return tuneNoteTo(69, freq); }

	/**
	 * The Tuning class is the primary place where you will interact with this library.
	 * It is constructed for a scale and mapping and then gives you the ability to
	 * determine frequencies across and beyond the midi keyboard. Since modulation
	 * can force key number well outside the [0,127] range in some of our synths we
	 * support a midi note range from -256 to + 256 spanning more than the entire frequency
	 * space reasonable.
	 *
	 * To use this class, you construct a fresh instance every time you want to use a
	 * different Scale and Keyboard. If you want to tune to a different scale or mapping,
	 * just construct a new instance.
	 */
	class Tuning {
		public:
			// The number of notes we pre-compute
			constexpr static int N = 512;

			inline Tuning(const Scale &s_, const KeyboardMapping &k_, bool allowTuningCenterOnUnmapped_)
			: allowTuningCenterOnUnmapped(allowTuningCenterOnUnmapped_) {
				// Shadow on purpose to make sure we use the modified version from rotation - use for dev
				// int *scale{0}, keyboardMapping{0};
				this->scale = s_;
				this->keyboardMapping = k_;
				Scale s = s_;
				KeyboardMapping k = k_;
				int oSP;
				if (s.count <= 0)
					throw TuningError(
						"Unable to tune to a scale with no notes. Your scale provided " +
						std::to_string(s.count) + " notes."
					);
				int useMiddleNote{k.middleNote};
				if (k.count > 0) {
					// Is the KBM not spanning the tuning note
					auto mapStart = useMiddleNote;
					auto mapEnd = useMiddleNote + k.count;
					while (mapStart > k.tuningConstantNote)	{
						useMiddleNote -= k.count;
						mapStart = useMiddleNote;
						mapEnd = useMiddleNote + k.count;
						// throw std::logic_error("Blah");
					}
					while (mapEnd < k.tuningConstantNote)	{
						useMiddleNote += k.count;
						mapStart = useMiddleNote;
						mapEnd = useMiddleNote + k.count;
						// throw std::logic_error("Blah");
					}
				}
				int kbmRotations{1};
				for (const auto &kv : k.keys) {
					kbmRotations = std::max(kbmRotations, (int)std::ceil(1.0 * kv / s.count));
				}
				if (kbmRotations > 1) {
					// This means the KBM has mapped note 5 in a 4 note scale or some such
					// which implies an 'unwrap' operation. So what we are going to do is
					// create a new scale which is extended then update the kbm octave position
					// accordingly.
					Scale newS = s;
					newS.count = s.count * kbmRotations;
					auto backCents = s.tones.back().cents;
					auto pushOff = backCents;
					for (int i = 1; i < kbmRotations; ++i) {
						for (const auto &t : s.tones)	{
							Tunings::Tone tCopy = t;
							tCopy.type = Tone::kToneCents;
							tCopy.cents += pushOff;
							tCopy.floatValue = tCopy.cents / 1200.0 + 1;

							newS.tones.push_back(tCopy);
						}
						pushOff += backCents;
					}
					s = newS;
					k.octaveDegrees *= kbmRotations;
					if (k.octaveDegrees == 0)
						k.octaveDegrees = s.count;
				}
				// From the KBM Spec: When not all scale degrees need to be mapped, the size of the map can be
				// smaller than the size of the scale.
				if (k.octaveDegrees > s.count) {
					throw TuningError(
						"Unable to apply mapping of size " + std::to_string(k.octaveDegrees) 
						+	" to smaller scale of size " + std::to_string(s.count)
					);
				}
				int posPitch0 = 256 + k.tuningConstantNote;
				int posScale0 = 256 + useMiddleNote;
				double pitchMod = log(k.tuningPitch) / log(2) - 1;
				int scalePositionOfTuningNote = k.tuningConstantNote - useMiddleNote;
				if (k.count > 0) {
					while (scalePositionOfTuningNote >= k.count) {
						scalePositionOfTuningNote -= k.count;
					}
					while (scalePositionOfTuningNote < 0)	{
						scalePositionOfTuningNote += k.count;
					}
					oSP = scalePositionOfTuningNote;
					scalePositionOfTuningNote = k.keys[scalePositionOfTuningNote];
					if (scalePositionOfTuningNote == -1 && !allowTuningCenterOnUnmapped) {
						std::string s = "Keyboard mapping is tuning an unmapped key. ";
						s += "Your tuning mapping is mapping key " + std::to_string(k.tuningConstantNote) 
						  + " as " + "the tuning constant note, but that is scale note " + std::to_string(oSP) 
						  + " given your scale root of " + std::to_string(k.middleNote) 
						  + " which your mapping does not assign. Please set your tuning constant "
								"note to a mapped key.";
						throw TuningError(s);
					}
				}
				double tuningCenterPitchOffset;
				if (scalePositionOfTuningNote == 0)
					tuningCenterPitchOffset = 0;
				else {
					if (scalePositionOfTuningNote == -1 && allowTuningCenterOnUnmapped)	{
						int low, high;
						bool octave_up = false;
						bool octave_down = false;
						float pitch_high;
						float pitch_low;
						// find next closest mapped note
						for (int i = oSP - 1; i != oSP; i = (i - 1) % k.count) {
							if (k.keys[i] != -1) {
								low = k.keys[i];
								break;
							}
							if (i > oSP) {
								octave_down = true;
							}
						}
						for (int i = oSP + 1; i != oSP; i = (i + 1) % k.count) {
							if (k.keys[i] != -1) {
								high = k.keys[i];
								break;
							}
							if (i < oSP) {
								octave_up = true;
							}
						}
						// determine high and low pitches
						double dt = s.tones[s.count - 1].cents;
						pitch_low = octave_down 
							? s.tones[low - 1].cents - dt 
							: s.tones[low - 1].floatValue - 1.0;
						pitch_high = octave_up 
							? s.tones[high - 1].cents + dt 
							: s.tones[high - 1].floatValue - 1.0;
						tuningCenterPitchOffset = (pitch_high + pitch_low) / 2.f;
					}	else {
						double tshift = 0;
						double dt = s.tones[s.count - 1].floatValue - 1.0;
						while (scalePositionOfTuningNote < 0) {
							scalePositionOfTuningNote += s.count;
							tshift += dt;
						}
						while (scalePositionOfTuningNote > s.count)	{
							scalePositionOfTuningNote -= s.count;
							tshift -= dt;
						}
						if (scalePositionOfTuningNote == 0)
							tuningCenterPitchOffset = -tshift;
						else
							tuningCenterPitchOffset = s.tones[scalePositionOfTuningNote - 1].floatValue
								- 1.0 - tshift;
					}
				}
				double pitches[N];
				for (int i = 0; i < N; ++i)	{
					// TODO: ScaleCenter and PitchCenter are now two different notes.
					int distanceFromPitch0 = i - posPitch0;
					int distanceFromScale0 = i - posScale0;
					if (distanceFromPitch0 == 0) {
						pitches[i] = 1;
						lptable[i] = pitches[i] + pitchMod;
						ptable[i] = pow(2.0, lptable[i]);
						if (k.count > 0) {
							int mappingKey = distanceFromScale0 % k.count;
							if (mappingKey < 0)
								mappingKey += k.count;
							int cm = k.keys[mappingKey];
							if (!allowTuningCenterOnUnmapped && cm < 0)	{
								std::string s = "Keyboard mapping is tuning an unmapped key. ";
								s += "Your tuning mapping is mapping key " + std::to_string(posPitch0 - 256) 
									+ " as " + "the tuning constant note, but that is scale note " + std::to_string(mappingKey) 
									+ " given your scale root of " + std::to_string(k.middleNote) 
									+ " which your mapping does not assign. Please set your tuning constant "
										 "note to a mapped key.";
								throw TuningError(s);
							}
						}
						scalepositiontable[i] = scalePositionOfTuningNote % s.count;
						#if DEBUG_SCALES
							std::cout << "PITCH: i=" << i << " n=" << i - 256 << " p=" << pitches[i]
												<< " lp=" << lptable[i] << " tp=" << ptable[i]
												<< " fr=" << ptable[i] * 8.175798915 << std::endl;
						#endif
					}	else {
						/*
							We used to have this which assumed 1-12
							Now we have our note number, our distance from the
							center note, and the key remapping
							int rounds = (distanceFromScale0-1) / s.count;
							int thisRound = (distanceFromScale0-1) % s.count;
						*/
						int rounds;
						int thisRound;
						int disable = false;
						if (k.count == 0) {
							rounds = (distanceFromScale0 - 1) / s.count;
							thisRound = (distanceFromScale0 - 1) % s.count;
						}	else {
							/*
							** Now we have this situation. We are at note i so we
							** are m away from the center note which is distanceFromScale0
							**
							** If we mod that by the mapping size we know which note we are on
							*/
							int mappingKey = distanceFromScale0 % k.count;
							int rotations = 0;
							if (mappingKey < 0) {
								mappingKey += k.count;
							}
							// Now have we gone off the end
							int dt = distanceFromScale0;
							if (dt > 0)	{
								while (dt >= k.count)	{
									dt -= k.count;
									rotations++;
								}
							}	else {
								while (dt < 0) {
									dt += k.count;
									rotations--;
								}
							}
							int cm = k.keys[mappingKey];
							int push = 0;
							if (cm < 0) {
								disable = true;
							}	else {
								if (cm > s.count)	{
									throw TuningError(std::string(
										"Mapping KBM note longer than scale; key=" + std::to_string(cm) +
										" scale count=" + std::to_string(s.count)
									));
								}
								push = mappingKey - cm;
							}
							if (k.octaveDegrees > 0 && k.octaveDegrees != k.count) {
								rounds = rotations;
								thisRound = cm - 1;
								if (thisRound < 0) {
									thisRound = k.octaveDegrees - 1;
									rounds--;
								}
							}	else {
								rounds = (distanceFromScale0 - push - 1) / s.count;
								thisRound = (distanceFromScale0 - push - 1) % s.count;
							}
							#ifdef DEBUG_SCALES
								if (i > 256 + 53 && i < 265 + 85)
									std::cout << "MAPPING n=" << i - 256 << " pushes ds0=" << distanceFromScale0
														<< " cmc=" << k.count << " tr=" << thisRound << " r=" << rounds
														<< " mk=" << mappingKey << " cm=" << cm << " push=" << push
														<< " dis=" << disable << " mk-p-1=" << mappingKey - push - 1
														<< " rotations=" << rotations << " od=" << k.octaveDegrees
														<< std::endl;
							#endif
						}
						if (thisRound < 0) {
							thisRound += s.count;
							rounds -= 1;
						}
						if (disable) {
							pitches[i] = 0;
							scalepositiontable[i] = -1;
						}	else {
							pitches[i] = s.tones[thisRound].floatValue 
								+ rounds * (s.tones[s.count - 1].floatValue - 1.0) 
								- tuningCenterPitchOffset;
							scalepositiontable[i] = (thisRound + 1) % s.count;
						}
						lptable[i] = pitches[i] + pitchMod;
						ptable[i] = pow(2.0, pitches[i] + pitchMod);
						#if DEBUG_SCALES
							if (i > 296 && i < 340)
								std::cout << "PITCH: i=" << i << " n=" << i - 256 << " ds0=" << distanceFromScale0
													<< " dp0=" << distanceFromPitch0 << " r=" << rounds << " t=" << thisRound
													<< " p=" << pitches[i] << " t=" << s.tones[thisRound].floatValue
													<< " c=" << s.tones[thisRound].cents << " dis=" << disable
													<< " tp=" << ptable[i] << " fr=" << ptable[i] * 8.175798915 << " tcpo="
													<< tuningCenterPitchOffset
													//<< " l2p=" << log(otp)/log(2.0)
													//<< " l2p-p=" << log(otp)/log(2.0) - pitches[i] - rounds - 3
													<< std::endl;
						#endif
					}
				}

				/*
				 * Finally we may have constructed an invalid tuning
				 */
			}

			// Construct a tuning with even temperament and standard mapping
			inline Tuning() : Tuning(evenTemperament12NoteScale(), KeyboardMapping()) {}
			// Construct a tuning for a particular scale, mapping, or for both.
			inline Tuning(const Scale &s) : Tuning(s, KeyboardMapping()) {}
			inline Tuning(const KeyboardMapping &k) : Tuning(evenTemperament12NoteScale(), k) {}

			/*
			 * Skipped notes can either have nonsense values or interpolated values.
			 * The old API made the bad choice to have nonsense values which we retain
			 * for compatability, but this method will return a new tuning with correctly
			 * interpolated skipped notes.
			 */
			inline Tuning withSkippedNotesInterpolated() const {
				Tuning res = *this;
				for (int i = 1; i < N - 1; ++i)	{
					if (scalepositiontable[i] < 0) {
						int nxt = i + 1;
						int prv = i - 1;
						while (prv >= 0 && scalepositiontable[prv] < 0)
							prv--;
						while (nxt < N && scalepositiontable[nxt] < 0)
							nxt++;
						float dist = (float)(nxt - prv);
						float frac = (float)(i - prv) / dist;
						res.lptable[i] = (1.0 - frac) * lptable[prv] + frac * lptable[nxt];
						res.ptable[i] = pow(2.0, res.lptable[i]);
					}
				}
				return res;
			}

			/**
			 * These three related functions provide you the information you
			 * need to use this tuning.
			 *
			 * frequencyForMidiNote returns the Frequency in HZ for a given midi
			 * note. In standard tuning, FrequencyForMidiNote(69) will be 440
			 * and frequencyForMidiNote(60) will be 261.62 - the standard frequencies
			 * for A and middle C.
			 *
			 * frequencyForMidiNoteScaledByMidi0 returns the frequency but with the
			 * standard frequency of midi note 0 divided out. So in standard tuning
			 * frequencyForMidiNoteScaledByMidi0(0) = 1 and frequencyForMidiNoteScaledByMidi0(60) = 32
			 *
			 * Finally logScaledFrequencyForMidiNote returns the log base 2 of the scaled frequency.
			 * So logScaledFrequencyForMidiNote(0) = 0 and logScaledFrequencyForMidiNote(60) = 5.
			 *
			 * Both the frequency measures have the feature of doubling when frequency doubles
			 * (or when a standard octave is spanned), whereas the log one increase by 1 per frequency
			 * double.
			 *
			 * Depending on your internal pitch model, one of these three methods should allow you
			 * to calibrate your oscillators to the appropriate frequency based on the midi note
			 * at hand.
			 *
			 * The scalePositionForMidiNote returns the space in the logical scale. Note 0 is the root.
			 * It has a maxiumum value of count-1. Note that SCL files omit the root internally and so
			 * this logical scale position is off by 1 from the index in the tones array of the Scale data.
			 */
			inline double frequencyForMidiNote(int mn) const {
				auto mni = std::min(std::max(0, mn + 256), N - 1);
				return ptable[mni] * MIDI_0_FREQ;
			}

			inline double frequencyForMidiNoteScaledByMidi0(int mn) const {
				auto mni = std::min(std::max(0, mn + 256), N - 1);
				return ptable[mni];
			}

			inline double logScaledFrequencyForMidiNote(int mn) const {
				auto mni = std::min(std::max(0, mn + 256), N - 1);
				return lptable[mni];
			}

			inline double retuningFromEqualInCentsForMidiNote(int mn) const	{
				return retuningFromEqualInSemitonesForMidiNote(mn) * 100.0;
			}

			inline double retuningFromEqualInSemitonesForMidiNote(int mn) const {
				return logScaledFrequencyForMidiNote(mn) * 12 - mn;
			}

			inline int scalePositionForMidiNote(int mn) const	{
				auto mni = std::min(std::max(0, mn + 256), N - 1);
				return scalepositiontable[mni];
			}

			inline bool isMidiNoteMapped(int mn) const {
				auto mni = std::min(std::max(0, mn + 256), N - 1);
				return scalepositiontable[mni] >= 0;
			}

			// For convenience, the scale and mapping used to construct this are kept as public copies
			Scale scale;
			KeyboardMapping keyboardMapping;

		private:
			std::array<double, N> ptable, lptable;
			std::array<int, N> scalepositiontable;
			bool allowTuningCenterOnUnmapped{false};
	};
} // namespace Tunings
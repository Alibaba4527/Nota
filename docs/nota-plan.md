# Nota — product and engineering plan

Nota is a desktop app for writing piano music by clicking. One piano part, one braced grand staff (treble and bass). The first screen is empty staves. Notes come from clicks. Advanced engraving stays out of the way.

Audience for this plan: the person building it. Milestones 1–9 are the MVP. Milestone 10 is after the MVP.

## Decisions

- **C++20 and CMake.** One language and one build system for Windows, macOS, and Linux. C++20 is the dialect (`std::span`, `std::optional`, constexpr). No second build system.
- **Qt 6 Widgets, not QML.** The product is a custom staff you click, plus a few buttons. Widgets is the direct path: `QWidget`, `QPainter`, `QUndoStack`, tooltips, dialogs. QML would put a scene graph in front of code that needs pixel hit-testing.
- **Custom notation view.** Draw with `QPainter` on a `QWidget`. Do not use `QGraphicsView`: a graphics item per note is a second model and fights measure layout. Do not use Verovio, LilyPond, or a MuseScore library. Those render finished scores. Click-to-edit means this app owns both the hit targets and the pixels.
- **Model separate from the view.** Types are `Score`, one piano `Part`, `Measure`, `Voice`, `Note`. The model does not include widgets or paint code. `Pitch` is a MIDI number plus spelling. `Duration` is a rational where a whole note is 1.
- **MusicXML export is written in-tree, later.** No libmusicxml in v1. Qt's `QXmlStreamWriter` is enough. Export is milestone 8, not part of the first build.
- **Playback is later, and it is a small sampler.** FluidSynth with one piano SoundFont. If linking FluidSynth slips that milestone, play sampled notes through Qt audio instead. Playback is not in milestone 1.
- **Product scope.** Name is Nota. Piano only. Beginners get large click targets, few words, tooltips, and strong contrast. Extra options stay hidden. No second instrument, no page of engraving settings.

## 1. Tech stack and libraries

**C++20.** The score model is value types and plain functions. C++20 is enough; do not add a language subset or a framework on top of Qt.

**CMake 3.21 or newer.** `find_package(Qt6)` and per-target links stay readable as `src/` grows. Generate one executable named `nota`.

**Qt 6.4 or newer, Widgets (`Qt6::Widgets`).** Window, toolbar, buttons, `QUndoStack`, `QScrollArea`, `QSettings`, file dialogs, and `QPainter`. Use the system Qt. Do not vendor Qt, do not FetchContent Qt, do not copy Qt into the repo.

**Qt Core XML (`QXmlStreamWriter`).** In Qt 6 this header ships with Qt Core. MusicXML is one writer class in milestone 8. No libmusicxml and no general XML library.

**Qt Test (`Qt6::Test`), from milestone 2.** Unit-test the model only: duration sums, measure capacity, pitch spelling, undo commands. The staff is checked by running the app.

**Qt Print Support (`Qt6::PrintSupport`), milestone 10 only.** `QPdfWriter` is a `QPainter`. The same `paint` path as the screen writes the PDF. Do not add a PDF library.

**FluidSynth (`libfluidsynth`), milestone 7.** It loads one SoundFont and plays note-on / note-off. GM program 0 is acoustic grand. Debian/Ubuntu package `libfluidsynth-dev` plus `fluid-soundfont-gm` (`/usr/share/sounds/sf2/FluidR3_GM.sf2`). Override the path with env `NOTA_SF2`. Do not commit a SoundFont. If the file is missing, the app still runs and Play shows one status line.

**Nothing else.** No Boost, no notation font, no Verovio, no LilyPond, no MuseScore, no libmusicxml. Clefs, the brace, noteheads, stems, and flags are `QPainter` paths in the notation code.

## 2. Architecture and directory layout

### Model

The model is standard C++. No `QObject`, no `QWidget`. Qt types start at the document boundary.

```text
Pitch
  midi          int          0..127, middle C = 60
  step          int          0 = C … 6 = B
  alter         int          -1, 0, +1
  octave        int          scientific pitch, C4 = middle C
  // midi must match step + alter + octave. One function builds a Pitch; callers do not set fields ad hoc.

Duration
  num, den      int          whole note = 1/1; half = 1/2; quarter = 1/4; eighth = 1/8
  // Add and compare with cross-multiply. Reduce by gcd. No floating point. No std::ratio (that is compile-time only).

Note
  pitch         Pitch
  duration      Duration

Voice
  notes         vector<Note> in onset order
  // Onset of a note is the sum of durations before it. Not stored.

Measure
  beats         Duration     4/4 means beats = 1/1, because four quarters are one whole note
  treble        Voice        staff 0
  bass          Voice        staff 1

Part
  name          "Piano"
  measures      vector<Measure>

Score
  part          Part         exactly one
```

A 4/4 voice is full when its durations sum to 1. A new note that would pass 1 is refused.

There is no accidental button, key signature, rest, chord, tie, beam, triplet, dynamic, or lyric in the MVP. `alter` exists so spelling is real, but the UI writes naturals only (`alter = 0`). Staff position picks the letter and octave; MIDI is computed from that. MusicXML can still write `<step>`, `<alter>`, `<octave>`.

### Document boundary

`Document` is a `QObject` the widgets hold.

- Owns the `Score`, the `QUndoStack`, and the selection.
- Selection is a measure index, a staff (treble or bass), and a note index. Empty selection is allowed.
- Every score change is a `QUndoCommand`: append note, delete note, change pitch, change duration, add measure. Widgets do not mutate `Score` themselves.
- Commands store indices and the old value, and they run only through the stack (linear undo). Do not add stable note IDs in the MVP.
- Undo restores notes. It clears the selection. That keeps milestone 4 small.
- `Document` emits one `changed()` signal. The staff and the keyboard repaint from the model. They do not cache notes.

### Notation view

`NotationView` is a `QWidget` inside a `QScrollArea`. `paintEvent` calls `layout(Score)` then draws. Layout is a pure function of the score plus constants. Do not store positions in the model.

Constants, milestone 1:

- Staff space: 14 px. Line thickness: 1 px. Ink: `#1a1a1a` on paper `#f7f4ec`.
- Treble middle line = B4. Bass middle line = D3. One diatonic step is half a staff space.
- Left gutter ~80 px for the brace, both clefs, and the 4/4 numerals (plain text, first measure only).
- Measure content width starts at 160 px and grows with the notes in that measure.
- Start with four measures. Barlines between them and a final barline.

Draw: staff lines, barlines, brace (a thick curved path), treble clef, bass clef, notehead, stem, flag. Whole note: open head, no stem. Half: open head, stem. Quarter: filled head, stem. Eighth: filled head, stem, one flag. No beams. Stem direction follows the staff middle line. Ledger lines when the pitch needs them, up to three diatonic steps beyond each staff.

From milestone 2 onward, a note's x comes from its onset, not from its index. Quarters then happen to be even. Milestone 3 does not need a new layout rule.

### Hit testing

- x picks the measure. y picks the diatonic step and the staff.
- The note is appended to that voice. It is not dropped at the pixel x inside the measure. Duration layout owns horizontal position.
- Clicks on noteheads select. Clicks on empty staff positions insert the current duration. Delete removes the selection.
- Hit slop is half a staff space, and at least 12 px.
- Middle C sits in the gap between the staves. The upper half of the gap is treble C4. The lower half is bass C4. Same pitch, different voice.
- Ledger clicks are allowed within the three-step limit. Clicks farther away do nothing.

### On-screen keyboard

A second `QWidget` under the staff. Fixed range C3–C6. White keys at least 28 px wide; the row scrolls horizontally if the window is narrower. Label the C keys only. A click inserts the selected duration at the next onset in the current measure. MIDI 60 and above goes to the treble voice; below 60 goes to the bass. Same "measure is full" rule as staff clicks. The keyboard does not replace staff entry.

### Playback

Not a sequencer framework. A `QTimer` (~16 ms) walks a list of note-on and note-off events built from both voices. Onset is in whole notes. Seconds = onset × 4 × 60 / bpm, because the tempo is quarters per minute. Default bpm is 80. One tempo slider. Play and Stop. FluidSynth note-on / note-off on program 0. This clock is not sample-accurate; that is acceptable for the MVP.

### MusicXML

`QXmlStreamWriter`, score-partwise, version 4.0. One part, `id="P1"`. Attributes: `divisions` 2 (quarter = 2, eighth = 1, half = 4, whole = 8), key fifths 0, time 4/4, staves 2, clef 1 treble (G, line 2), clef 2 bass (F, line 4). Write treble notes as voice 1 staff 1, then `<backup>` for the measure, then bass notes as voice 2 staff 2. If a voice is short of the measure, write one rest for the remainder. No libmusicxml.

### PDF (after MVP)

`QPdfWriter` in landscape. Call the same layout and draw functions with a scale factor. No second renderer.

### Directory

Milestone 1 creates the tree and fills `ui/` and `notation/` only. Later milestones fill the other folders. Empty folders are not required before they have files.

```text
CMakeLists.txt
README.md
src/
  main.cpp
  ui/
    MainWindow.h / .cpp          toolbar, status line, owns Document
    DurationPalette.h / .cpp     milestone 3
    PianoKeyboard.h / .cpp       milestone 6
    FirstRunHint.h / .cpp        milestone 9
  notation/
    NotationView.h / .cpp
    Layout.h / .cpp              score in, positions out
    Glyphs.h / .cpp              clef, brace, notehead, flag paths
  model/
    Pitch.h
    Duration.h
    Note.h
    Voice.h
    Measure.h
    Part.h
    Score.h
    Document.h / .cpp            milestone 2, Qt boundary
    Commands.h / .cpp            milestone 2, undo types grow in milestone 4
  audio/
    Playback.h / .cpp            milestone 7
  io/
    MusicXmlWriter.h / .cpp      milestone 8
    PdfExporter.h / .cpp         milestone 10
tests/
  tst_model.cpp                  milestone 2
```

`CMakeLists.txt`: C++20, `CMAKE_AUTOMOC` on, `find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets)`. Later targets add `Test`, `PrintSupport`, and FluidSynth via pkg-config. Keep those `find_package` calls out until the milestone that uses them.

### Milestone 1 build shape

- System Qt only. Linux: `qt6-base-dev`, `cmake`, a C++20 compiler (`build-essential` on Debian/Ubuntu; `qt6-qtbase-devel` on Fedora). macOS: Homebrew `qt` or the Qt installer, plus CMake. Windows: Qt 6 SDK and CMake, MSVC or another C++20 compiler. Do not vendor Qt.
- README lists install packages and the commands below.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/nota
```

Window title is `Nota`. Binary name is `nota`.

## 3. Milestones

Each one is a single implementation pass. Stop at its "done" line. Do not pull the next milestone forward.

### 1. Empty grand staff

Window opens and draws a braced grand staff: treble clef, bass clef, five lines each, 4/4 numerals, barlines, four empty measures. No model types, no mouse input, no menus.

Done when: the README commands build and launch on Linux against system Qt 6; resizing the window redraws the staff; the binary links the distro Qt and does not carry a copy of Qt.

### 2. Quarter notes

Add the model, `Document`, and append/delete commands. A new document has four empty 4/4 measures. Click a staff position to append a quarter in that measure and voice. Click a notehead to select it (blue outline `#1d4ed8`). Delete removes it. A click that would overflow the measure does nothing.

Done when: treble and bass each accept four quarters; a fifth quarter in that voice does not appear; delete removes the selected note; `tst_model` covers duration sum, 4/4 capacity, and MIDI matching the spelling.

### 3. Duration palette

Four large buttons: whole, half, quarter, eighth. The active one is visibly selected. The next insert uses it. Drawing follows the duration rules in section 2. Insert still refuses when the note does not fit (a whole note only fits in an empty 4/4 voice).

Done when: each duration can be entered on either staff and is drawn with the right head, stem, and flag, spaced by onset.

### 4. Edit pitch and duration, undo and redo

Up and Down move the selected note one diatonic step and rewrite the spelling as a natural, including ledger lines inside the three-step limit. A palette click with a selection changes that note's duration when the measure still fits; otherwise the duration stays. Undo and redo cover insert, delete, pitch, and duration. Toolbar buttons plus Ctrl+Z and Ctrl+Shift+Z (Cmd on macOS).

Done when: those four edits undo and redo in order, and a duration change that would overflow does not change the note and does not push an undo command.

### 5. Measures and scrolling

One control: Add measure. It appends one empty 4/4 measure. `NotationView` reports its full width to the scroll area. The scroll is horizontal. New measures take clicks the same way as the first four.

Done when: adding measures past the window width scrolls to them, and a new measure accepts a note.

### 6. On-screen piano

The C3–C6 keyboard in section 2. Keys use the selected duration and the same full-measure rule. A one-line status message ("This measure is full") is enough feedback.

Done when: clicking a labeled C inserts that pitch on the correct staff (C4 treble), and a key below C4 inserts on the bass staff.

### 7. Playback

Play, Stop, and a tempo slider defaulting to quarter = 80. FluidSynth, GM piano, one SoundFont, path from section 1. Both voices sound in time. Missing SoundFont: status line, no crash, editing still works.

Done when: a short treble-and-bass example plays and Stop silences it.

### 8. Export MusicXML

Save dialog, default extension `.musicxml`, writer from section 2. Manual check: open the file in a MusicXML reader and confirm piano, treble, bass, 4/4, and the written rhythms. Rests fill leftover voice time.

Done when: a score with notes on both staves round-trips into a reader as a grand staff with the same pitches and durations.

### 9. Beginner UI pass

This pass does not add musical features. Tooltips on the palette, keyboard, add measure, undo, redo, play, stop, and export. Toolbar only; no engraving menus. Buttons at least 40 px tall. Contrast stays paper `#f7f4ec` and ink `#1a1a1a`. First launch shows one hint: "Click a line or space to add a note." OK dismisses it. `QSettings` (org `Nota`, app `Nota`) remembers that, and the hint does not return.

Done when: a second launch does not show the hint, and every toolbar control has a tooltip.

### 10. PDF export — after the MVP

The MVP is milestones 1–9. PDF is next, not part of calling the MVP done.

Save dialog, `.pdf`, landscape, `QPdfWriter`, same layout and glyphs as the screen. More than one page only if the system count does not fit one page.

Done when: the PDF shows the same notes, clefs, and barlines as the window.

## Out of scope

Until a later plan says otherwise: accidentals and key signatures, rests as editable objects, chords, ties, beams, tuplets, dynamics, pedal, lyrics, multiple parts, guitar or other clefs, system wrapping (scroll sideways instead), page layout, MusicXML import, and a bundled copy of Qt.

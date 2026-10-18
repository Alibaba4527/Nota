// Model tests: duration sums, 4/4 capacity, and MIDI matching the staff spelling.

#include "model/Score.h"

#include <QtTest>

class TestModel : public QObject {
    Q_OBJECT

private slots:
    void durationSumsAndReduces();
    void fourQuartersFillTheMeasure();
    void fifthQuarterIsRefused();
    void otherVoiceStaysOpen();
    void staffPositionMatchesMidi();
};

void TestModel::durationSumsAndReduces() {
    const nota::Duration quarter = nota::duration(1, 4);
    const nota::Duration half = quarter + quarter;
    QCOMPARE(half.num, 1);
    QCOMPARE(half.den, 2);

    const nota::Duration three = half + quarter;
    QCOMPARE(three.num, 3);
    QCOMPARE(three.den, 4);

    const nota::Duration whole = three + quarter;
    QCOMPARE(whole.num, 1);
    QCOMPARE(whole.den, 1);

    const nota::Duration reduced = nota::duration(2, 8);
    QCOMPARE(reduced.num, 1);
    QCOMPARE(reduced.den, 4);

    QCOMPARE(nota::compare(nota::duration(1, 4), nota::duration(1, 2)), -1);
    QCOMPARE(nota::compare(nota::duration(2, 4), nota::duration(1, 2)), 0);
    QCOMPARE(nota::compare(nota::duration(3, 4), nota::duration(1, 2)), 1);
    QCOMPARE(nota::compare(whole, nota::wholeDuration()), 0);
}

void TestModel::fourQuartersFillTheMeasure() {
    nota::Measure measure;
    const nota::Duration quarter = nota::quarterDuration();
    for (int i = 0; i < 4; ++i) {
        QVERIFY(nota::accepts(measure, nota::Staff::Treble, quarter));
        measure.treble.notes.push_back(nota::Note{nota::pitchOnStaff(nota::Staff::Treble, 0), quarter});
    }
    QCOMPARE(nota::compare(nota::voiceDuration(measure.treble), measure.beats), 0);
    QVERIFY(nota::accepts(measure, nota::Staff::Bass, quarter));
}

void TestModel::fifthQuarterIsRefused() {
    nota::Measure measure;
    const nota::Duration quarter = nota::quarterDuration();
    for (int i = 0; i < 4; ++i) {
        measure.treble.notes.push_back(nota::Note{nota::pitch(0, 0, 4), quarter});
    }
    QVERIFY(!nota::accepts(measure, nota::Staff::Treble, quarter));
    QCOMPARE(nota::compare(nota::voiceDuration(measure.treble) + quarter, measure.beats), 1);
}

void TestModel::otherVoiceStaysOpen() {
    nota::Score score = nota::emptyPianoScore();
    QCOMPARE(score.part.name, std::string("Piano"));
    QCOMPARE(static_cast<int>(score.part.measures.size()), 4);

    nota::Measure& measure = score.part.measures[0];
    const nota::Duration quarter = nota::quarterDuration();
    for (int i = 0; i < 4; ++i) {
        QVERIFY(nota::accepts(measure, nota::Staff::Bass, quarter));
        measure.bass.notes.push_back(nota::Note{nota::pitchOnStaff(nota::Staff::Bass, 0), quarter});
    }
    QVERIFY(!nota::accepts(measure, nota::Staff::Bass, quarter));
    QVERIFY(nota::accepts(measure, nota::Staff::Treble, quarter));
    QVERIFY(nota::accepts(score.part.measures[1], nota::Staff::Bass, quarter));
}

void TestModel::staffPositionMatchesMidi() {
    const nota::Pitch trebleMiddle = nota::pitchOnStaff(nota::Staff::Treble, 0);
    QCOMPARE(trebleMiddle.step, 6);
    QCOMPARE(trebleMiddle.alter, 0);
    QCOMPARE(trebleMiddle.octave, 4);
    QCOMPARE(trebleMiddle.midi, 71);
    QCOMPARE(trebleMiddle.midi, nota::pitch(6, 0, 4).midi);

    const nota::Pitch bassMiddle = nota::pitchOnStaff(nota::Staff::Bass, 0);
    QCOMPARE(bassMiddle.step, 1);
    QCOMPARE(bassMiddle.alter, 0);
    QCOMPARE(bassMiddle.octave, 3);
    QCOMPARE(bassMiddle.midi, 50);

    const nota::Pitch trebleMiddleC = nota::pitchOnStaff(nota::Staff::Treble, -6);
    QCOMPARE(trebleMiddleC.step, 0);
    QCOMPARE(trebleMiddleC.octave, 4);
    QCOMPARE(trebleMiddleC.alter, 0);
    QCOMPARE(trebleMiddleC.midi, 60);

    const nota::Pitch bassMiddleC = nota::pitchOnStaff(nota::Staff::Bass, 6);
    QCOMPARE(bassMiddleC.midi, 60);
    QCOMPARE(bassMiddleC.step, 0);
    QCOMPARE(bassMiddleC.octave, 4);

    const nota::Pitch c5 = nota::pitchOnStaff(nota::Staff::Treble, 1);
    QCOMPARE(c5.step, 0);
    QCOMPARE(c5.octave, 5);
    QCOMPARE(c5.midi, nota::pitch(0, 0, 5).midi);

    QCOMPARE(nota::pitch(3, 1, 4).midi, 66);
    QCOMPARE(nota::pitch(6, -1, 3).midi, 58);
}

QTEST_GUILESS_MAIN(TestModel)
#include "tst_model.moc"

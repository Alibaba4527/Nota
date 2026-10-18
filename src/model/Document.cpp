// Qt boundary for the score. Owns the score, the undo stack, and the selection.
// Widgets do not mutate the score. Append and delete run as commands on the stack.

#include "model/Document.h"

#include "model/Commands.h"

namespace nota {

Document::Document(QObject* parent) : QObject(parent), score_(emptyPianoScore()), undo_(this) {}

void Document::select(Selection selection) {
    selection_ = selection;
    notify();
}

void Document::appendQuarter(int measure, Staff staff, Pitch spelled) {
    if (measure < 0 || measure >= static_cast<int>(score_.part.measures.size())) {
        return;
    }
    const Measure& target = score_.part.measures[static_cast<size_t>(measure)];
    const Duration quarter = quarterDuration();
    if (!accepts(target, staff, quarter)) {
        return;
    }
    undo_.push(new AppendNoteCommand(this, measure, staff, Note{spelled, quarter}));
}

void Document::deleteSelection() {
    if (selection_.empty() || selection_.measure < 0
        || selection_.measure >= static_cast<int>(score_.part.measures.size())) {
        return;
    }
    Measure& target = score_.part.measures[static_cast<size_t>(selection_.measure)];
    Voice& voice = voiceOf(target, selection_.staff);
    if (selection_.note < 0 || selection_.note >= static_cast<int>(voice.notes.size())) {
        return;
    }
    const Note removed = voice.notes[static_cast<size_t>(selection_.note)];
    undo_.push(new DeleteNoteCommand(this, selection_.measure, selection_.staff, selection_.note, removed));
}

void Document::notify() {
    emit changed();
}

} // namespace nota

// Append and delete commands. They store indices and the old note.
// Undo puts the note back and clears the selection. Pitch and duration edits come later.

#include "model/Commands.h"

AppendNoteCommand::AppendNoteCommand(nota::Document* document, int measure, nota::Staff staff, nota::Note note)
    : document_(document), measure_(measure), staff_(staff), note_(note) {
    setText(QStringLiteral("Append note"));
}

void AppendNoteCommand::redo() {
    auto& notes = nota::voiceOf(document_->score_.part.measures[static_cast<size_t>(measure_)], staff_).notes;
    notes.push_back(note_);
    document_->selection_ = nota::Selection{measure_, staff_, static_cast<int>(notes.size()) - 1};
    document_->notify();
}

void AppendNoteCommand::undo() {
    auto& notes = nota::voiceOf(document_->score_.part.measures[static_cast<size_t>(measure_)], staff_).notes;
    if (!notes.empty()) {
        notes.pop_back();
    }
    document_->selection_ = {};
    document_->notify();
}

DeleteNoteCommand::DeleteNoteCommand(nota::Document* document, int measure, nota::Staff staff, int index,
                                     nota::Note note)
    : document_(document), measure_(measure), staff_(staff), index_(index), note_(note) {
    setText(QStringLiteral("Delete note"));
}

void DeleteNoteCommand::redo() {
    auto& notes = nota::voiceOf(document_->score_.part.measures[static_cast<size_t>(measure_)], staff_).notes;
    if (index_ >= 0 && index_ < static_cast<int>(notes.size())) {
        notes.erase(notes.begin() + index_);
    }
    document_->selection_ = {};
    document_->notify();
}

void DeleteNoteCommand::undo() {
    auto& notes = nota::voiceOf(document_->score_.part.measures[static_cast<size_t>(measure_)], staff_).notes;
    const int index = index_ < 0 ? 0 : index_;
    if (index > static_cast<int>(notes.size())) {
        notes.push_back(note_);
    } else {
        notes.insert(notes.begin() + index, note_);
    }
    document_->selection_ = {};
    document_->notify();
}

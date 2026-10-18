// Append and delete commands. They store indices and the old note.
// Undo puts the note back and clears the selection. Pitch and duration edits come later.

#pragma once

#include "model/Document.h"

#include <QUndoCommand>

class AppendNoteCommand : public QUndoCommand {
public:
    AppendNoteCommand(nota::Document* document, int measure, nota::Staff staff, nota::Note note);

    void redo() override;
    void undo() override;

private:
    nota::Document* document_;
    int measure_;
    nota::Staff staff_;
    nota::Note note_;
};

class DeleteNoteCommand : public QUndoCommand {
public:
    DeleteNoteCommand(nota::Document* document, int measure, nota::Staff staff, int index, nota::Note note);

    void redo() override;
    void undo() override;

private:
    nota::Document* document_;
    int measure_;
    nota::Staff staff_;
    int index_;
    nota::Note note_;
};

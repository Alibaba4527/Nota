// Qt boundary for the score. Owns the score, the undo stack, and the selection.
// Widgets do not mutate the score. Append and delete run as commands on the stack.

#pragma once

#include "model/Score.h"

#include <QObject>
#include <QUndoStack>

class AppendNoteCommand;
class DeleteNoteCommand;

namespace nota {

struct Selection {
    int measure = -1;
    Staff staff = Staff::Treble;
    int note = -1;

    [[nodiscard]] bool empty() const { return measure < 0 || note < 0; }
};

class Document : public QObject {
    Q_OBJECT

public:
    explicit Document(QObject* parent = nullptr);

    [[nodiscard]] const Score& score() const { return score_; }
    [[nodiscard]] Selection selection() const { return selection_; }

    void select(Selection selection);
    void appendQuarter(int measure, Staff staff, Pitch spelled);
    void deleteSelection();

signals:
    void changed();

private:
    friend class ::AppendNoteCommand;
    friend class ::DeleteNoteCommand;

    void notify();

    Score score_;
    QUndoStack undo_;
    Selection selection_;
};

} // namespace nota

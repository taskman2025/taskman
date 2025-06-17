#include "taskman/frontends/PosixProcessItemModel.h"
#include "taskman/platform_profiles/posix/process_field_bits.h"
#include <QColor>

QVariant PosixProcessItemModel::customData(QModelIndex const& index, int role, proc_id_t pid, ProcessData const& proc) const {
    QList<ProcessField> const& fields = getProcessFields();

    // TODO: role tooltip: display what each state is, e.g. S = Sleeping, R = Running...

    if (role == Qt::BackgroundRole) {
        int col = index.column();
        if (col >= 0 && col < fields.size()) {
            field_mask_t fieldBit = fields[col].mask;
            if (fieldBit == PosixProcessFieldBit::STATE) {
                char state = proc.getFieldValue(PosixProcessFieldBit::STATE).toChar().toUpper().toLatin1();
                switch (state) {
                case 'R':
                    return QColor("#C8E6C9"); // light green
                case 'S':
                    return QColor("#BBDEFB"); // light blue
                case 'D':
                    return QColor("#C5CAE9"); // light indigo
                case 'T':
                    return QColor("#FFE0B2"); // light orange
                case 't':
                    return QColor("#FFCCBC"); // light deep orange
                case 'Z':
                    return QColor("#FFCDD2"); // light red
                case 'X':
                    return QColor("#EEEEEE"); // light gray
                case 'x':
                    return QColor("#EEEEEE"); // light gray
                case 'K':
                    return QColor("#E1BEE7"); // light purple
                case 'W':
                    return QColor("#B2EBF2"); // light cyan
                case 'P':
                    return QColor("#D7CCC8"); // light brown
                case 'I':
                    return QColor("#CFD8DC"); // light blue gray
                default:
                    return QColor("#F5F5F5"); // very light gray for unknown
                }
            }
        }
    } else if (role == Qt::ForegroundRole) {
        int col = index.column();
        if (col >= 0 && col < fields.size()) {
            field_mask_t fieldBit = fields[col].mask;
            if (fieldBit == PosixProcessFieldBit::STATE) {
                char state = proc.getFieldValue(PosixProcessFieldBit::STATE).toChar().toUpper().toLatin1();
                switch (state) {
                case 'R':
                    return QColor("#1B5E20"); // dark green
                case 'S':
                    return QColor("#0D47A1"); // dark blue
                case 'D':
                    return QColor("#1A237E"); // dark indigo
                case 'T':
                    return QColor("#E65100"); // dark orange
                case 't':
                    return QColor("#BF360C"); // dark deep orange
                case 'Z':
                    return QColor("#B71C1C"); // dark red
                case 'X':
                    return QColor("#424242"); // dark gray
                case 'x':
                    return QColor("#424242"); // dark gray
                case 'K':
                    return QColor("#4A148C"); // dark purple
                case 'W':
                    return QColor("#006064"); // dark cyan
                case 'P':
                    return QColor("#3E2723"); // dark brown
                case 'I':
                    return QColor("#263238"); // dark blue gray
                default:
                    return QColor("#212121"); // dark gray for unknown
                }
            }
        }
    }

    return BaseProcessItemModel::customData(index, role, pid, proc);
}

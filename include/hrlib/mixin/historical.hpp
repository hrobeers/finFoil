/****************************************************************************

 Copyright (c) 2014, Hans Robeers
 All rights reserved.

 BSD 2-Clause License

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
following conditions are met:

   * Redistributions of source code must retain the above copyright notice, this list of conditions and the following 
disclaimer.

   * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the 
following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE
 COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

****************************************************************************/

#ifndef HRLIB_HISTORICAL_H
#define HRLIB_HISTORICAL_H

#include <QList>
#include <QStringList>
#include <QUuid>

#define Q_PROPERTY_HISTORY_STRLIST Q_PROPERTY(QStringList history READ historyStrList WRITE setHistory RESET resetHistory) // QStringList for compacter serialization

namespace hrlib
{
    class Historical
    {
    private:
        int _historyCount = 0;
        QList<QUuid> _history;

        void trim()
        {
            if (_historyCount > 0)
            {
                while(_history.count() > _historyCount)
                    _history.removeFirst();
            }
        }

    public:
        Historical() { }
        Historical(int historyCount) : _historyCount(historyCount) {}

        int historyCount() { return _historyCount; }
        void setHistoryCount(int historyCount) { _historyCount = historyCount; }

        QList<QUuid> history() { trim(); return _history; }
        void setHistory(QList<QUuid> history) { _history = history; }
        void resetHistory() { _history.clear(); }

        QStringList historyStrList()
        {
            QStringList retVal;

            foreach (const QUuid &id, history())
                retVal.append(id.toString());

            return retVal;
        }

        void setHistory(QStringList history)
        {
            _history.clear();
            foreach (const QString &id, history)
            {
                QUuid uuid(id);
                if (!uuid.isNull())
                    _history.append(uuid);
            }
        }
    };

    template <int HistoryCount>
    class THistorical : public Historical
    {
    public: THistorical() : Historical(HistoryCount) {}
    };
}

#endif // HRLIB_HISTORICAL_H

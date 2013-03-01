/****************************************************************************
  
 Copyright (c) 2013, Hans Robeers
 All rights reserved.
 
 BSD 2-Clause License
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
   * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   
   * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
   
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
****************************************************************************/

#include <QCoreApplication>
#include <QDir>
#include <QHash>
#include <QDebug>

#include "hrlib/unittesting.h"

using namespace hrlib;

// TODO move to internal logic into unittesting.cpp
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    bool runAllTests = true;
    Q_FOREACH(const QString& arg, app.arguments()) {
        if (arg.startsWith("-dir=")) {
            // Run in this directory
            QDir::setCurrent(arg.mid(5));
        } else if (arg.startsWith("-test=")) {
            // Run this test
            QString testName(arg.mid(6));

            if (!TestFactoryBase::getTests().contains(testName)) {
                qDebug() << "Test" << testName << "not registered";
                return -1;
            }

            QObject* test = TestFactoryBase::getTests().value(testName)->createTest();
            const int ret = QTest::qExec(test);
            delete test;
            if (ret != 0)
                return ret;

            // We have run a test manually, so don't run them all
            runAllTests = false;
        }
    }

    if (runAllTests) {
        QHash<QString, TestFactoryBase*>& tests(TestFactoryBase::getTests());
        for (QHash<QString, TestFactoryBase*>::const_iterator i = tests.constBegin(); i != tests.constEnd(); ++i) {
            QObject* test = i.value()->createTest();
            const int ret = QTest::qExec(test);
            delete test;
            if (ret != 0) {
                return ret;
            }
        }
    }

    return 0;
}

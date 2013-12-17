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

#ifndef UNITTESTING_H
#define UNITTESTING_H

#include <QTest>
#include <QDir>
#include <QDebug>

namespace hrlib
{
    class TestFactoryBase
    {
    public:
        virtual ~TestFactoryBase() {}

        virtual QObject* createTest() = 0;
    };

    class UnitTests
    {
    public:
        static QHash<QString, TestFactoryBase*>& getTests()
        {
            static QHash<QString, TestFactoryBase*> tests;
            return tests;
        }

        static void addTest(TestFactoryBase* test, const char* testName)
        {
            UnitTests::getTests().insert(QString(testName), test);
        }

        static int runTests(QCoreApplication& app)
        {
            bool runAllTests = true;
            Q_FOREACH(const QString& arg, app.arguments()) {
                if (arg.startsWith("-dir=")) {
                    // Run in this directory
                    QDir::setCurrent(arg.mid(5));
                } else if (arg.startsWith("-test=")) {
                    // Run this test
                    QString testName(arg.mid(6));

                    if (!UnitTests::getTests().contains(testName)) {
                        qDebug() << "Test" << testName << "not registered";
                        return -1;
                    }

                    QObject* test = UnitTests::getTests().value(testName)->createTest();
                    const int ret = QTest::qExec(test);
                    delete test;
                    if (ret != 0)
                        return ret;

                    // We have run a test manually, so don't run them all
                    runAllTests = false;
                }
            }

            if (runAllTests) {
                QHash<QString, TestFactoryBase*>& tests(UnitTests::getTests());
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
    };

    template <typename T> class TestFactory : public TestFactoryBase
    {
    public:
        TestFactory(const char* testName)
        {
            UnitTests::addTest(this, testName);
        }

        virtual QObject* createTest() { return new T; }
    };
}

#define HR_ADD_TEST(testClass) static hrlib::TestFactory<testClass> testFactory(#testClass);

// This one is missing from the Qt test lib
#define HR_ASSERT_THROW(code, exc) \
    try { \
        code; \
        QFAIL("Exception #exc not thrown"); \
    } catch (exc&) { \
        /* All is well */ \
    }

#endif // UNITTESTING_H

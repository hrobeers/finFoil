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

namespace hrlib {
    class TestFactoryBase {
    public:
        virtual ~TestFactoryBase() {}

        virtual QObject* createTest() = 0;

        static QHash<QString, TestFactoryBase*>& getTests()
        {
            static QHash<QString, TestFactoryBase*> tests;
            return tests;
        }
    };

    void addTest(TestFactoryBase* test, const char* testName);
    int runTests(QCoreApplication& app);

    template <typename T> class TestFactory : public TestFactoryBase {
    public:
        TestFactory(const char* testName)
        {
            addTest(this, testName);
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

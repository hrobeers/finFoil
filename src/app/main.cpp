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

#define MAIN_CPP
#include "app/main.hpp"

#include <QApplication>
#include <QCommandLineParser>

#include <QDir>

QTextStream out(stdout);
QTextStream err(stderr);

#include "app/about.hpp"
#include "app/runinteractive.hpp"

int main(int argc, char *argv[])
{
    try
    {
#ifdef EXECUTE_WITHIN_APP_BUNDLE
        // restricting library-path to bundle
        QDir dir(argv[0]);          // e.g. appdir/Contents/MacOS/appname
        assert(dir.cdUp());
        assert(dir.cdUp());
        assert(dir.cd("PlugIns"));  // e.g. appdir/Contents/PlugIns
        QCoreApplication::setLibraryPaths(QStringList(dir.absolutePath()));
#endif
        QApplication app(argc, argv);
        QApplication::setApplicationName("finFoil");
        QApplication::setApplicationVersion(version.toString());

        QCommandLineParser parser;
        parser.setApplicationDescription(about());
        parser.addHelpOption();
        parser.addVersionOption();

#ifndef WEB_DISABLED
        QCommandLineOption serverUrl(QStringLiteral("server-url"),
                                     QApplication::tr("Base URL of the finFoil server."),
                                     QStringLiteral("URL"));
        parser.addOption(serverUrl);
#endif

        parser.addPositionalArgument(QStringLiteral("file"), QApplication::tr("File to open."));

        // Process the actual command line arguments given by the user
        parser.process(app);
        const QStringList args = parser.positionalArguments();

        // Read the file path
        QString filePath;
        if (!args.isEmpty())
            filePath = args.first();

        QUrl baseUrl;
#ifndef WEB_DISABLED
        if (parser.isSet(serverUrl))
            baseUrl = QUrl::fromUserInput(parser.value(serverUrl));
#endif
        if (!baseUrl.isValid())
            baseUrl = QStringLiteral("https://finfoil.io/s");

        return runInteractive(app, baseUrl, filePath);
    }
    catch (std::exception &ex)
    {
        err << ex.what() << endl;
        return 1;
    }
}

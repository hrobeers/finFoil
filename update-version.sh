#!/bin/sh

commitNum=$(git log --oneline | wc -l | tr -d ' ')
commitHash=$(git rev-parse --verify HEAD)

echo "// This file is auto-generated" > src/version_autogen.h
echo "// Any changes made to this file will be lost" >> src/version_autogen.h
echo "// Update this file using update-version.sh" >> src/version_autogen.h
echo "" >> src/version_autogen.h
echo "#ifndef VERSION_AUTOGEN_H" >> src/version_autogen.h
echo "#define VERSION_AUTOGEN_H" >> src/version_autogen.h
echo "" >> src/version_autogen.h
echo "#define BUILD_NUMBER $commitNum" >> src/version_autogen.h
echo "#define COMMIT_HASH \"$commitHash\"" >> src/version_autogen.h
echo "" >> src/version_autogen.h
echo "#endif // VERSION_AUTOGEN_H" >> src/version_autogen.h
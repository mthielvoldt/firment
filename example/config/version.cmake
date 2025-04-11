# Read the version from an external file then strip any whitespace
file(READ "config/version.txt" VERSION_CONTENT)
string(STRIP "${VERSION_CONTENT}" VERSION_SEM)
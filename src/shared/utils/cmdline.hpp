#pragma once

#include <string>

namespace fr {

/**
 * Retrieves the value of a flag on the command line. If either the short
 * form or long form are empty strings, they are ignored.
 *
 * @param   argc        The argc value passed into main.
 * @param   argv        The argv value passed into main.
 * @param   flag_short  The short form ("-f") of the flag to look for.
 * @param   flag_long   The long form ("--flag") of the flag to look for.
 * @return  The argument passed with that flag. Empty string if there was
 *          no argument.
 */
std::string FlagValue(int argc, char *argv[],
 const std::string& flag_short, const std::string& flag_long);

/**
 * Checks for the existence of the the passed flag on the command line. If
 * either the short form or long form are empty strings, they are ignored.
 *
 * @param   argc        The argc value passed into main.
 * @param   argv        The argv value passed into main.
 * @param   flag_short  The short form ("-f") of the flag to look for.
 * @param   flag_long   The long form ("--flag") of the flag to look for.
 * @return  True if the flag is present, false otherwise.
 */
bool FlagExists(int argc, char *argv[], const std::string& flag_short,
 const std::string& flag_long);

/**
 * Retrieves the value of the i'th argument on the command line. If the i'th
 * argument does not exist, returns an empty string. Arguments are counted
 * as monotonically increasing integers starting at 1, ignoring interspersed
 * flags. The 0'th argument is the name of the executable.
 *
 * @param   argc    The argc value passed into main.
 * @param   argv    The argv value passed into main.
 * @param   i       Which command line argument to retrieve.
 * @return  The value of the i'th command line argument, or an empty string if
 *          the i'th argument does not exist.
 */
std::string ArgumentValue(int argc, char* argv[], int i);

} // namespace fr

/// \file Stringify.h
/// \author G. Christian
/// \brief Macro trick to turn a #define into a string literal.
#ifndef UTILS_STRINGIFY_INCLUDE_GUARD
#define UTILS_STRINGIFY_INCLUDE_GUARD

/// Macro to turn a #define into a string literal
/*!
 * Example:
 * \code
 * #define A_MACRO filename.txt
 * std::cout "The name of the file is: " << DRAGON_UTILS_STRINGIFY(A_MACRO) << "\n";
 * \endcode
 *
 * Result: "The name of the file is filename.txt"
 */
#define DRAGON_UTILS_STRINGIFY(S) DRAGON_UTILS_STRINGIFY_HELPER (S)

/// Helper macro for DRAGON_UTILS_STRINGIFY
#define DRAGON_UTILS_STRINGIFY_HELPER(S) #S


#endif

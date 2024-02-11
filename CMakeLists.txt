# CMakeList.txt: projekt CMake dla elementu Effective-algorithms, dołącz źródło i zdefiniuj
# tutaj logikę specyficzną dla projektu.
#
cmake_minimum_required (VERSION 3.8)

# Włącz przeładowywanie kodu XAML na gorąco dla kompilatorów MSVC, jeśli jest to obsługiwane.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Effective-algorithms")

# Dodaj źródło do pliku wykonywalnego tego projektu.
add_executable (Effective-algorithms "src/main.cpp" "include/menu.hpp" "src/util.cpp" "src/tabuSearch.cpp" "src/algorithms.cpp" "src/menu.cpp" "src/geneticAlgorithm.cpp" "src/bruteForce.cpp" 
"src/dynamicProgramming.cpp" "src/simulatedAnnealing.cpp")
target_include_directories(Effective-algorithms PRIVATE "include")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Effective-algorithms PROPERTY CXX_STANDARD 20)
endif()

# TODO: Dodaj testy i zainstaluj elementy docelowe w razie potrzeby.

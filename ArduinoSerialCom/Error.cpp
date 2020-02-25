#include "Error.hpp"

Error::Error() {
}

void Error::setError(errorMessages _error) {
  error = _error;
}

char const* Error::getError() {
  switch (error) {
    case CONNECTION_ERROR:
      return "EROARE_LA_CONECTARE";
    case READ_ERROR:
      return "EROARE_LA_CITIRE";
    case WRITE_ERROR:
      return "EROARE_LA_SCRIERE";
    default:
      return "UNKNOWN_ERROR";
  }
}

// char const* Error::errorMessageToString(errorMessage _error) {
//   switch (_error) {
//     case CONNECTION_ERROR:
//       return "EROARE_LA_CONECTARE";
//     case READ_ERROR:
//       return "EROARE_LA_CITIRE";
//     case WRITE_ERROR:
//       return "EROARE_LA_SCRIERE";
//     default:
//       return "UNKNOWN_ERROR";
//   }
// }

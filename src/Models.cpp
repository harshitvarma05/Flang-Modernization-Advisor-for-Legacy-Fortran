#include "Models.hpp"

std::string toString(Effort effort) {
  switch (effort) {
  case Effort::Trivial:
    return "trivial";
  case Effort::Moderate:
    return "moderate";
  case Effort::Complex:
    return "complex";
  }
  return "moderate";
}

std::string toString(Safety safety) {
  switch (safety) {
  case Safety::Safe:
    return "safe";
  case Safety::ReviewNeeded:
    return "review-needed";
  case Safety::Risky:
    return "risky";
  }
  return "review-needed";
}

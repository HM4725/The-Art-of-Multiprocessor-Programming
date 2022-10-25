#pragma once

#include <string>

template <typename T> struct Invoc {
public:
  std::string method;
  T arg;
  Invoc() : method("bogus") {}
  Invoc(std::string method) : method(method) {}
  Invoc(std::string method, T arg) : method(method), arg(arg) {}
  Invoc(const Invoc &invoc) : method(invoc.method), arg(invoc.arg) {}
};

template <typename R> struct Response {
public:
  bool empty;
  R v;
  Response() : empty(true) {}
  Response(R v) : empty(false), v(v) {}
};

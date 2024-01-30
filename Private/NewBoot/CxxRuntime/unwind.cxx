namespace cxxkit {
///! @brief C++ ABI unwinding
///! finis array (r1)
///! n of finis (r2)
extern "C" void __unwind(void (**finis)(void), int cnt) {
  for (int i = 0; i < cnt; ++i) (finis[i])();
}
}  // namespace cxxkit

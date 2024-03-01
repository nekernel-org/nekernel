namespace cxxkit {
///! @brief C++ ABI unwinding
///! Fini array (r1)
///! Numbers of Fini (r2)
extern "C" void __unwind(void (**finis)(void), int cnt) {
  for (int i = 0; i < cnt; ++i) (finis[i])();
}
}  // namespace cxxkit

-- Lexing ... OK
+
+
@@ Tokens @@
List[82]:
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  sizeof
  const char('a')
  !=
  const int(4)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  sizeof
  const int(2147483647)
  !=
  const int(4)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  sizeof
  const unsigned int(4294967295U)
  !=
  const int(4)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  sizeof
  const long(2l)
  !=
  const int(8)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  sizeof
  const unsigned long(0ul)
  !=
  const int(8)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  sizeof
  const double(1.0)
  !=
  const int(8)
  )
  {
  return
  const int(6)
  ;
  }
  return
  const int(0)
  ;
  }

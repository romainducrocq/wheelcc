-- Lexing ... OK
+
+
@@ Tokens @@
List[123]:
  unsigned
  identifier(u)
  ;
  int
  unsigned
  identifier(u)
  ;
  unsigned
  int
  identifier(u)
  =
  const int(6)
  ;
  unsigned
  long
  identifier(ul)
  ;
  long
  unsigned
  identifier(ul)
  ;
  long
  int
  unsigned
  identifier(ul)
  ;
  unsigned
  int
  long
  identifier(ul)
  =
  const int(4)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(u)
  !=
  const unsigned int(6u)
  )
  {
  return
  const int(1)
  ;
  }
  long
  extern
  unsigned
  identifier(ul)
  ;
  unsigned
  long
  extern
  identifier(ul)
  ;
  int
  extern
  unsigned
  long
  identifier(ul)
  ;
  if
  (
  identifier(ul)
  !=
  const unsigned long(4ul)
  )
  {
  return
  const int(2)
  ;
  }
  int
  identifier(counter)
  =
  const int(0)
  ;
  for
  (
  unsigned
  int
  identifier(index)
  =
  const int(10)
  ;
  identifier(index)
  <
  const unsigned int(4294967295U)
  ;
  identifier(index)
  =
  identifier(index)
  -
  const int(1)
  )
  {
  identifier(counter)
  =
  identifier(counter)
  +
  const int(1)
  ;
  }
  if
  (
  identifier(counter)
  !=
  const int(11)
  )
  {
  return
  const int(3)
  ;
  }
  return
  const int(0)
  ;
  }

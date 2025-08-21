-- Lexing ... OK
+
+
@@ Tokens @@
List[101]:
  extern
  unsigned
  int
  identifier(ui)
  ;
  unsigned
  int
  identifier(return_uint)
  (
  void
  )
  ;
  int
  identifier(return_uint_as_signed)
  (
  void
  )
  ;
  long
  identifier(return_uint_as_long)
  (
  void
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(ui)
  !=
  const unsigned int(4294967200u)
  )
  return
  const int(0)
  ;
  identifier(ui)
  =
  -
  const int(1)
  ;
  long
  identifier(result)
  =
  (
  long
  )
  identifier(return_uint)
  (
  )
  ;
  if
  (
  identifier(result)
  !=
  const long(4294967295l)
  )
  return
  const int(0)
  ;
  identifier(result)
  =
  (
  long
  )
  identifier(return_uint_as_signed)
  (
  )
  ;
  if
  (
  identifier(result)
  !=
  -
  const long(1l)
  )
  return
  const int(0)
  ;
  identifier(result)
  =
  identifier(return_uint_as_long)
  (
  )
  ;
  if
  (
  identifier(result)
  !=
  const long(4294967295l)
  )
  return
  const int(0)
  ;
  return
  const int(1)
  ;
  }

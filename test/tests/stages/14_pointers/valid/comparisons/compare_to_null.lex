-- Lexing ... OK
+
+
@@ Tokens @@
List[89]:
  double
  *
  identifier(get_null_pointer)
  (
  void
  )
  {
  return
  const int(0)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  double
  identifier(x)
  ;
  double
  *
  identifier(null)
  =
  identifier(get_null_pointer)
  (
  )
  ;
  double
  *
  identifier(non_null)
  =
  &
  identifier(x)
  ;
  if
  (
  identifier(non_null)
  ==
  const int(0)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  !
  (
  identifier(null)
  ==
  const long(0l)
  )
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  !
  (
  identifier(non_null)
  !=
  const unsigned int(0u)
  )
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(null)
  !=
  const unsigned long(0ul)
  )
  {
  return
  const int(4)
  ;
  }
  return
  const int(0)
  ;
  }

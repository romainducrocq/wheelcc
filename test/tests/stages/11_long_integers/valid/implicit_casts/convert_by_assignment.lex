-- Lexing ... OK
+
+
@@ Tokens @@
List[123]:
  int
  identifier(return_truncated_long)
  (
  long
  identifier(l)
  )
  {
  return
  identifier(l)
  ;
  }
  long
  identifier(return_extended_int)
  (
  int
  identifier(i)
  )
  {
  return
  identifier(i)
  ;
  }
  int
  identifier(truncate_on_assignment)
  (
  long
  identifier(l)
  ,
  int
  identifier(expected)
  )
  {
  int
  identifier(result)
  =
  identifier(l)
  ;
  return
  identifier(result)
  ==
  identifier(expected)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  long
  identifier(result)
  =
  identifier(return_truncated_long)
  (
  const long(4294967298l)
  )
  ;
  if
  (
  identifier(result)
  !=
  const long(2l)
  )
  {
  return
  const int(1)
  ;
  }
  identifier(result)
  =
  identifier(return_extended_int)
  (
  -
  const int(10)
  )
  ;
  if
  (
  identifier(result)
  !=
  -
  const int(10)
  )
  {
  return
  const int(2)
  ;
  }
  int
  identifier(i)
  =
  const long(4294967298l)
  ;
  if
  (
  identifier(i)
  !=
  const int(2)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  !
  identifier(truncate_on_assignment)
  (
  const long(17179869184l)
  ,
  const int(0)
  )
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

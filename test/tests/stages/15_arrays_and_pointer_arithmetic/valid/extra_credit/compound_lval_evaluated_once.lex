-- Lexing ... OK
+
+
@@ Tokens @@
List[104]:
  int
  identifier(get_call_count)
  (
  void
  )
  {
  static
  int
  identifier(count)
  =
  const int(0)
  ;
  identifier(count)
  +=
  const int(1)
  ;
  return
  identifier(count)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(arr)
  [
  const int(4)
  ]
  =
  {
  const int(10)
  ,
  const int(11)
  ,
  const int(12)
  ,
  const int(13)
  }
  ;
  if
  (
  identifier(arr)
  [
  identifier(get_call_count)
  (
  )
  ]
  !=
  const int(11)
  )
  {
  return
  const int(1)
  ;
  }
  int
  *
  identifier(end_ptr)
  =
  identifier(arr)
  +
  const int(4)
  ;
  if
  (
  (
  identifier(end_ptr)
  -
  const int(1)
  )
  [
  -
  identifier(get_call_count)
  (
  )
  ]
  !=
  const int(11)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(get_call_count)
  (
  )
  !=
  const int(3)
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

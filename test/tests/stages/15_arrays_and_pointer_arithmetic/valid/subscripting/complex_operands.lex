-- Lexing ... OK
+
+
@@ Tokens @@
List[446]:
  int
  identifier(assign_in_index)
  (
  int
  identifier(idx)
  )
  {
  int
  identifier(arr)
  [
  const int(3)
  ]
  =
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  }
  ;
  int
  identifier(val)
  =
  identifier(arr)
  [
  identifier(idx)
  =
  identifier(idx)
  +
  const int(2)
  ]
  ;
  if
  (
  identifier(idx)
  !=
  const int(1)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(val)
  !=
  const int(2)
  )
  {
  return
  const int(2)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(static_index)
  (
  void
  )
  {
  static
  int
  identifier(index)
  =
  const int(0)
  ;
  int
  identifier(retval)
  =
  identifier(index)
  ;
  identifier(index)
  =
  identifier(index)
  +
  const int(1)
  ;
  return
  identifier(retval)
  ;
  }
  int
  identifier(funcall_in_index)
  (
  void
  )
  {
  int
  identifier(arr)
  [
  const int(3)
  ]
  =
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  }
  ;
  int
  identifier(v1)
  =
  identifier(arr)
  [
  identifier(static_index)
  (
  )
  ]
  ;
  int
  identifier(v2)
  =
  identifier(arr)
  [
  identifier(static_index)
  (
  )
  ]
  ;
  if
  (
  identifier(v1)
  !=
  const int(1)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(v2)
  !=
  const int(2)
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
  int
  identifier(subscript_inception)
  (
  long
  *
  identifier(arr)
  ,
  int
  *
  identifier(a)
  ,
  int
  identifier(b)
  )
  {
  return
  identifier(arr)
  [
  identifier(a)
  [
  identifier(b)
  ]
  ]
  ;
  }
  int
  identifier(check_subscript_inception)
  (
  void
  )
  {
  long
  identifier(arr)
  [
  const int(4)
  ]
  =
  {
  const int(4)
  ,
  const int(3)
  ,
  const int(2)
  ,
  const int(1)
  }
  ;
  int
  identifier(indices)
  [
  const int(2)
  ]
  =
  {
  const int(1)
  ,
  const int(2)
  }
  ;
  if
  (
  identifier(subscript_inception)
  (
  identifier(arr)
  ,
  identifier(indices)
  ,
  const int(1)
  )
  !=
  const int(2)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(subscript_inception)
  (
  identifier(arr)
  ,
  identifier(indices)
  ,
  const int(0)
  )
  !=
  const int(3)
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
  int
  *
  identifier(get_array)
  (
  void
  )
  {
  static
  int
  identifier(arr)
  [
  const int(3)
  ]
  ;
  return
  identifier(arr)
  ;
  }
  int
  identifier(subscript_function_result)
  (
  void
  )
  {
  identifier(get_array)
  (
  )
  [
  const int(2)
  ]
  =
  const int(1)
  ;
  if
  (
  identifier(get_array)
  (
  )
  [
  const int(2)
  ]
  !=
  const int(1)
  )
  {
  return
  const int(7)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(negate_subscript)
  (
  int
  *
  identifier(arr)
  ,
  int
  identifier(idx)
  ,
  int
  identifier(expected)
  )
  {
  if
  (
  identifier(arr)
  [
  -
  identifier(idx)
  ]
  !=
  identifier(expected)
  )
  {
  return
  const int(8)
  ;
  }
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
  int
  identifier(check)
  =
  identifier(assign_in_index)
  (
  -
  const int(1)
  )
  ;
  if
  (
  identifier(check)
  )
  {
  return
  identifier(check)
  ;
  }
  identifier(check)
  =
  identifier(funcall_in_index)
  (
  )
  ;
  if
  (
  identifier(check)
  )
  {
  return
  identifier(check)
  ;
  }
  identifier(check)
  =
  identifier(check_subscript_inception)
  (
  )
  ;
  if
  (
  identifier(check)
  )
  {
  return
  identifier(check)
  ;
  }
  identifier(check)
  =
  identifier(subscript_function_result)
  (
  )
  ;
  if
  (
  identifier(check)
  )
  {
  return
  identifier(check)
  ;
  }
  int
  identifier(arr)
  [
  const int(3)
  ]
  =
  {
  const int(0)
  ,
  const int(1)
  ,
  const int(2)
  }
  ;
  identifier(check)
  =
  identifier(negate_subscript)
  (
  identifier(arr)
  +
  const int(2)
  ,
  const int(2)
  ,
  const int(0)
  )
  ;
  if
  (
  identifier(check)
  )
  {
  return
  identifier(check)
  ;
  }
  return
  const int(0)
  ;
  }

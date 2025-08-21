-- Lexing ... OK
+
+
@@ Tokens @@
List[143]:
  struct
  identifier(inner)
  {
  long
  identifier(l)
  ;
  char
  identifier(arr)
  [
  const int(2)
  ]
  ;
  }
  ;
  struct
  identifier(outer)
  {
  char
  identifier(a)
  ;
  struct
  identifier(inner)
  identifier(b)
  ;
  }
  ;
  int
  identifier(validate_struct_array)
  (
  struct
  identifier(outer)
  *
  identifier(struct_array)
  )
  ;
  int
  identifier(validate_struct_array)
  (
  struct
  identifier(outer)
  *
  identifier(struct_array)
  )
  {
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(3)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  if
  (
  identifier(struct_array)
  [
  identifier(i)
  ]
  .
  identifier(a)
  !=
  identifier(i)
  *
  const int(2)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(struct_array)
  [
  identifier(i)
  ]
  .
  identifier(b)
  .
  identifier(l)
  !=
  identifier(i)
  *
  const int(3)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(struct_array)
  [
  identifier(i)
  ]
  .
  identifier(b)
  .
  identifier(arr)
  [
  const int(0)
  ]
  !=
  identifier(i)
  *
  const int(4)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(struct_array)
  [
  identifier(i)
  ]
  .
  identifier(b)
  .
  identifier(arr)
  [
  const int(1)
  ]
  !=
  identifier(i)
  *
  const int(5)
  )
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }

-- Lexing ... OK
+
+
@@ Tokens @@
List[159]:
  struct
  identifier(ten_bytes)
  {
  char
  identifier(arr)
  [
  const int(10)
  ]
  ;
  }
  ;
  extern
  struct
  identifier(ten_bytes)
  identifier(on_page_boundary)
  ;
  struct
  identifier(ten_bytes)
  identifier(return_struct)
  (
  void
  )
  {
  identifier(on_page_boundary)
  .
  identifier(arr)
  [
  const int(9)
  ]
  =
  -
  const int(1)
  ;
  identifier(on_page_boundary)
  .
  identifier(arr)
  [
  const int(8)
  ]
  =
  -
  const int(2)
  ;
  identifier(on_page_boundary)
  .
  identifier(arr)
  [
  const int(7)
  ]
  =
  -
  const int(3)
  ;
  return
  identifier(on_page_boundary)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  struct
  identifier(ten_bytes)
  identifier(x)
  =
  identifier(return_struct)
  (
  )
  ;
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(7)
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
  identifier(x)
  .
  identifier(arr)
  [
  identifier(i)
  ]
  )
  {
  return
  const int(1)
  ;
  }
  }
  if
  (
  identifier(x)
  .
  identifier(arr)
  [
  const int(7)
  ]
  !=
  -
  const int(3)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(x)
  .
  identifier(arr)
  [
  const int(8)
  ]
  !=
  -
  const int(2)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(x)
  .
  identifier(arr)
  [
  const int(9)
  ]
  !=
  -
  const int(1)
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

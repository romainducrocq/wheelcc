-- Lexing ... OK
+
+
@@ Tokens @@
List[210]:
  struct
  identifier(eighteen_bytes)
  {
  char
  identifier(arr)
  [
  const int(18)
  ]
  ;
  }
  ;
  extern
  struct
  identifier(eighteen_bytes)
  identifier(on_page_boundary)
  ;
  struct
  identifier(eighteen_bytes)
  identifier(return_struct)
  (
  void
  )
  {
  identifier(on_page_boundary)
  .
  identifier(arr)
  [
  const int(17)
  ]
  =
  const int(12)
  ;
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
  identifier(eighteen_bytes)
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
  const int(18)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  char
  identifier(val)
  =
  identifier(x)
  .
  identifier(arr)
  [
  identifier(i)
  ]
  ;
  if
  (
  identifier(i)
  ==
  const int(7)
  )
  {
  if
  (
  identifier(val)
  !=
  -
  const int(3)
  )
  {
  return
  const int(1)
  ;
  }
  }
  else
  if
  (
  identifier(i)
  ==
  const int(8)
  )
  {
  if
  (
  identifier(val)
  !=
  -
  const int(2)
  )
  {
  return
  const int(2)
  ;
  }
  }
  else
  if
  (
  identifier(i)
  ==
  const int(9)
  )
  {
  if
  (
  identifier(val)
  !=
  -
  const int(1)
  )
  {
  return
  const int(3)
  ;
  }
  }
  else
  if
  (
  identifier(i)
  ==
  const int(17)
  )
  {
  if
  (
  identifier(val)
  !=
  const int(12)
  )
  {
  return
  const int(4)
  ;
  }
  }
  else
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
  const int(5)
  ;
  }
  }
  return
  const int(0)
  ;
  }

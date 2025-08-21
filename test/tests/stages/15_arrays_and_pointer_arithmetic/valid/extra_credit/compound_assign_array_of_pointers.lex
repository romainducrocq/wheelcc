-- Lexing ... OK
+
+
@@ Tokens @@
List[190]:
  int
  identifier(main)
  (
  void
  )
  {
  static
  int
  (
  *
  identifier(array_of_pointers)
  [
  const int(3)
  ]
  )
  [
  const int(4)
  ]
  =
  {
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  }
  ;
  int
  identifier(array1)
  [
  const int(4)
  ]
  =
  {
  const int(100)
  ,
  const int(101)
  ,
  const int(102)
  ,
  const int(103)
  }
  ;
  int
  identifier(nested_array)
  [
  const int(2)
  ]
  [
  const int(4)
  ]
  =
  {
  {
  const int(200)
  ,
  const int(201)
  ,
  const int(202)
  ,
  const int(203)
  }
  ,
  {
  const int(300)
  ,
  const int(301)
  ,
  const int(302)
  ,
  const int(303)
  }
  }
  ;
  identifier(array_of_pointers)
  [
  const int(0)
  ]
  =
  &
  identifier(array1)
  ;
  identifier(array_of_pointers)
  [
  const int(1)
  ]
  =
  &
  identifier(nested_array)
  [
  const int(0)
  ]
  ;
  identifier(array_of_pointers)
  [
  const int(2)
  ]
  =
  &
  identifier(nested_array)
  [
  const int(1)
  ]
  ;
  identifier(array_of_pointers)
  [
  const int(0)
  ]
  +=
  const int(1)
  ;
  if
  (
  identifier(array_of_pointers)
  [
  const int(0)
  ]
  [
  -
  const int(1)
  ]
  [
  const int(3)
  ]
  !=
  const int(103)
  )
  {
  return
  const int(1)
  ;
  }
  identifier(array_of_pointers)
  [
  const int(1)
  ]
  +=
  const int(1)
  ;
  identifier(array_of_pointers)
  [
  const int(2)
  ]
  -=
  const int(1)
  ;
  if
  (
  identifier(array_of_pointers)
  [
  const int(1)
  ]
  [
  const int(0)
  ]
  [
  const int(3)
  ]
  !=
  const int(303)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(array_of_pointers)
  [
  const int(2)
  ]
  [
  const int(0)
  ]
  [
  const int(3)
  ]
  !=
  const int(203)
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

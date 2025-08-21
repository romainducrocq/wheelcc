-- Lexing ... OK
+
+
@@ Tokens @@
List[147]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(switch1)
  =
  const int(0)
  ;
  int
  identifier(switch2)
  =
  const int(0)
  ;
  int
  identifier(switch3)
  =
  const int(0)
  ;
  switch
  (
  const int(3)
  )
  {
  case
  const int(0)
  :
  return
  const int(0)
  ;
  case
  const int(1)
  :
  if
  (
  const int(0)
  )
  {
  case
  const int(3)
  :
  identifier(switch1)
  =
  const int(1)
  ;
  break
  ;
  }
  default
  :
  return
  const int(0)
  ;
  }
  switch
  (
  const int(4)
  )
  {
  case
  const int(0)
  :
  return
  const int(0)
  ;
  if
  (
  const int(1)
  )
  {
  return
  const int(0)
  ;
  }
  else
  {
  case
  const int(4)
  :
  identifier(switch2)
  =
  const int(1)
  ;
  break
  ;
  }
  default
  :
  return
  const int(0)
  ;
  }
  switch
  (
  const int(5)
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
  const int(10)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  identifier(switch1)
  =
  const int(0)
  ;
  case
  const int(5)
  :
  identifier(switch3)
  =
  const int(1)
  ;
  break
  ;
  default
  :
  return
  const int(0)
  ;
  }
  }
  return
  (
  identifier(switch1)
  &&
  identifier(switch2)
  &&
  identifier(switch3)
  )
  ;
  }

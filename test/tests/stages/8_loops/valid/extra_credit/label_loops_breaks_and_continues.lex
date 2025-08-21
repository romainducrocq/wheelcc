-- Lexing ... OK
+
+
@@ Tokens @@
List[109]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(sum)
  =
  const int(0)
  ;
  goto
  identifier(do_label)
  ;
  return
  const int(0)
  ;
  identifier(do_label)
  :
  do
  {
  identifier(sum)
  =
  const int(1)
  ;
  goto
  identifier(while_label)
  ;
  }
  while
  (
  const int(1)
  )
  ;
  identifier(while_label)
  :
  while
  (
  const int(1)
  )
  {
  identifier(sum)
  =
  identifier(sum)
  +
  const int(1)
  ;
  goto
  identifier(break_label)
  ;
  return
  const int(0)
  ;
  identifier(break_label)
  :
  break
  ;
  }
  ;
  goto
  identifier(for_label)
  ;
  return
  const int(0)
  ;
  identifier(for_label)
  :
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
  identifier(sum)
  =
  identifier(sum)
  +
  const int(1)
  ;
  goto
  identifier(continue_label)
  ;
  return
  const int(0)
  ;
  identifier(continue_label)
  :
  continue
  ;
  return
  const int(0)
  ;
  }
  return
  identifier(sum)
  ;
  }

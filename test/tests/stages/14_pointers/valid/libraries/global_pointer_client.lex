-- Lexing ... OK
+
+
@@ Tokens @@
List[41]:
  extern
  double
  *
  identifier(d_ptr)
  ;
  int
  identifier(update_thru_ptr)
  (
  double
  identifier(new_val)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  double
  identifier(d)
  =
  const double(0.0)
  ;
  identifier(d_ptr)
  =
  &
  identifier(d)
  ;
  identifier(update_thru_ptr)
  (
  const double(10.0)
  )
  ;
  return
  (
  identifier(d)
  ==
  const double(10.0)
  )
  ;
  }

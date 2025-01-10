fn main()
{
  let y =
  {
    let x = 3;
    x + 1
  }
  // blocks are expressions
  // (list of statements followed by expression) 
  // if no final expression: value is ()
  println!("{x}");
}

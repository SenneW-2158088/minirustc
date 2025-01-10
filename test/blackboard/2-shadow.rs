fn main()
{
  let bananas = 5;
  {
    let bananas = bananas + 3;
    println!(bananas);
  }
  println!(bananas);
}

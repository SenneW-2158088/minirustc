fn main() {
    println!("test");
    let mut a = 0;
    loop {
        if a == 10 {
            break;
        }

        a += 1;
    }
    println!(a);
}

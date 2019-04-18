function determineDecimalSeparator() {
    let n = 1.1;

    let result = /^1(.+)1$/.exec(n.toLocaleString())[1];

    return n;
}

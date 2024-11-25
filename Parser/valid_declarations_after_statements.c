Program DeclarationsAfterStatements {
    int x;
    x = 5;
    {
        x = x + 1;
        int y;
        y = x * 2;
        x = y - 3;
    }
}


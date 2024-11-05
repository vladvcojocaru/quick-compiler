function sumUpTo(n : int) : int
    var sum : int;
    var counter : int;

    sum = 0;
    counter = 1;
    
    while (counter < n)
        sum = sum + counter;
        counter = counter + 1;
    end

    return sum;
end



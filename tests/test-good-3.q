function checkEligibility(age : int, hasLicense : int) : str
    var status : str;
    
    if (age < 18 || hasLicense == 0)
        status = "Not eligible to drive";
    else
        status = "Eligible to drive";
    end
    
    return status;
end


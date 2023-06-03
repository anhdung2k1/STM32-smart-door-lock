package com.example.authentication.services.interfaces;

import com.example.authentication.builder.AuthenticationResponse;
import com.example.authentication.exception.AccountNotFoundException;
import com.example.authentication.model.Accounts;

public interface AccountService {
    AuthenticationResponse createAccount(Accounts account) throws Exception;

    AuthenticationResponse authenticate(Accounts account) throws AccountNotFoundException;
}

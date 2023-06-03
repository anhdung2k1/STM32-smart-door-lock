package com.example.authentication.controller;

import com.example.authentication.builder.AuthenticationResponse;
import com.example.authentication.exception.AccountNotFoundException;
import com.example.authentication.model.Accounts;
import com.example.authentication.services.interfaces.AccountService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

/*
    @author Anh Dung
 */
@CrossOrigin(origins = "http://192.168.80.129:5000")
@RestController
@RequestMapping("/api")
public class AccountsController {
    @Autowired
    private final AccountService accountService;

    public AccountsController(AccountService accountService) {
        this.accountService = accountService;
    }

    // Sign up
    @PostMapping("/accounts/signup")
    // Perform create a new account -> Gen new id
    // We need a handle token as well
    public ResponseEntity<AuthenticationResponse> createAccount(@RequestBody Accounts account) throws Exception {
        return ResponseEntity.ok(
                accountService.createAccount(account));
    }

    // SIGN IN
    @PostMapping("/accounts/signin")
    public ResponseEntity<AuthenticationResponse> authenticate(@RequestBody Accounts account)
            throws AccountNotFoundException {
        return ResponseEntity.ok(accountService.authenticate(account));
    }
}

package com.example.authentication.services.implement;

import java.time.LocalDateTime;
import java.util.NoSuchElementException;

import org.springframework.beans.BeanUtils;
import org.springframework.security.authentication.AuthenticationManager;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.stereotype.Service;

import com.example.authentication.builder.AccountBuilder;
import com.example.authentication.builder.AuthenticationResponse;
import com.example.authentication.config.JwtService;
import com.example.authentication.entity.AccountsEntity;
import com.example.authentication.entity.UsersEntity;
import com.example.authentication.exception.AccountNotFoundException;
import com.example.authentication.model.Accounts;
import com.example.authentication.repository.AccountBuilderRepository;
import com.example.authentication.repository.AccountsRepository;
import com.example.authentication.repository.UsersRepository;
import com.example.authentication.services.interfaces.AccountService;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.context.SecurityContextHolder;
import jakarta.transaction.Transactional;
import lombok.RequiredArgsConstructor;

@Service
@Transactional(rollbackOn = Exception.class)
@RequiredArgsConstructor
public class AccountServiceImpl implements AccountService {
    private final AccountsRepository accountsRepository;
    private final UsersRepository usersRepository;
    private final AccountBuilderRepository accountBuilderRepository;
    private final JwtService jwtService;
    private final AuthenticationManager authenticationManager;
    private final PasswordEncoder passwordEncoder;

    @Override
    public AuthenticationResponse createAccount(Accounts account) throws Exception {
        try {
            // Tạo một Account Entity Constructor
            AccountsEntity accountsEntity = new AccountsEntity();
            // Kiểm tra account có trong database hay k ? Nếu có thì throw exception
            if (accountsRepository.findByUserName(account.getUserName()).isPresent()) {
                throw new Exception("User exists");
            }
            // Set all the parameters from front-end login by the USER ROLE
            // Hashed Password when create new account
            String encodedPassword = passwordEncoder.encode(account.getHashPassword());
            account.setHashPassword(encodedPassword);
            // Default set
            account.setPhone_number(0L);
            account.setCreateAt(LocalDateTime.now());
            account.setUpdateAt(LocalDateTime.now());
            // Khi tạo 1 Entity mới cần phải lưu vào DB trước khi flush -> Gen user trc khi
            // tạo foreign key trong Account
            UsersEntity users = new UsersEntity(account.getUserName());
            usersRepository.save(users);
            account.setUsers(users);
            // Copy tất cả những thuộc tính còn lại qua account Entity --> Save vào database
            BeanUtils.copyProperties(account, accountsEntity);
            accountsRepository.save(accountsEntity);
            // Tạo một Builder mới dùng để tạo token xác thực
            var user = AccountBuilder.builder()
                    .name(account.getUserName())
                    .hashPassword(encodedPassword)
                    .phone_number(account.getPhone_number())
                    .createAt(account.getCreateAt())
                    .updateAt(account.getUpdateAt())
                    .build();
            // Tạo token
            var jwtToken = jwtService.generateToken(user);
            // Trả về token và builder result account khi tạo 1 user mới xong
            return AuthenticationResponse.builder().token(jwtToken)
                    .build();
        } catch (Exception e) {
            throw new Exception(e.getMessage());
        }
    }

    @Override
    public AuthenticationResponse authenticate(Accounts account) throws AccountNotFoundException {
        try {
            Authentication authentication = authenticationManager.authenticate(
                    new UsernamePasswordAuthenticationToken(
                            account.getUserName(),
                            account.getHashPassword()));
            SecurityContextHolder.getContext().setAuthentication(authentication);
            AccountBuilder user = accountBuilderRepository.findByUserName(account.getUserName())
                    .orElseThrow();
            var jwtToken = jwtService.generateToken(user);
            return AuthenticationResponse.builder().token(jwtToken).build();
        } catch (NoSuchElementException e) {
            throw new AccountNotFoundException("User not found");
        }
    }
}

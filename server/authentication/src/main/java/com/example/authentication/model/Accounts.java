package com.example.authentication.model;

import java.time.LocalDateTime;

import com.example.authentication.entity.UsersEntity;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/*
    @author Anh Dung
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class Accounts {
    private Long acc_id;
    private String userName;
    private String hashPassword;
    private Long phone_number;
    private UsersEntity users;
    private LocalDateTime createAt;
    private LocalDateTime updateAt;
}
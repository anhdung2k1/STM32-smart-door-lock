package com.example.authentication.repository;

import com.example.authentication.entity.AccountsEntity;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.stereotype.Repository;

import java.util.Optional;

@Repository
public interface AccountsRepository extends JpaRepository<AccountsEntity, Long> {
    @Query(value = "select ac.* from accounts ac where ac.user_name=:userName", nativeQuery = true)
    Optional<AccountsEntity> findByUserName(String userName);
}
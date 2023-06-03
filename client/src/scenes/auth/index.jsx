import React,{ useState } from 'react';
import Input from './input';
import {Box, Grow, Avatar, Button, Paper, Grid, Typography, Container} from '@mui/material';
import { useDispatch } from 'react-redux';
import {useNavigate} from 'react-router-dom';
import {signin, signup} from '../../actions/auth';
import LockIcon from '@mui/icons-material/Lock';

const initialState = {
    userName: "",
    hashPassword: "",
    confirmPassword: "",
};
const Auth = () => {
    const [showPassword, setShowPassword] = useState(false);
    const [isSignUp, setIsSignUp] = useState(false);
    const [formData, setFormData] = useState(initialState);
    const dispatch = useDispatch();
    const navigate = useNavigate();
    const handleSubmit = (e) => {
        e.preventDefault();
        if(isSignUp && (formData.hashPassword === formData.confirmPassword)){
        dispatch(signup(formData,navigate));
        }
        else{
        dispatch(signin(formData,navigate));
        }
    }
    const handleChange = (e) => {
        setFormData({ ...formData, [e.target.name]: e.target.value})
    }
    const switchMode = () => {
        setIsSignUp((prevIsSignUp) => !prevIsSignUp);
        setShowPassword(false);
    }
    const handleShowPassword = () => setShowPassword((prevShowPassword) => !prevShowPassword);
    return (
        <Grow in>
            <Container component="main" maxWidth="xl">
             <Container component = "main" maxWidth = "xs">
                <Paper sx = {{display: 'flex', flexDirection: 'column', alignItems: 'center', backgroundColor: 'gray'}}elevation={3}> 
                    <Avatar sx = {{margin: '5px',backgroundColor: 'white'}}>
                        <LockIcon />
                    </Avatar>
                    <Typography variant = "h5">{isSignUp ? 'Sign Up' : 'Sign In'}</Typography>
                        <form sx = {{width: '100%', marginTop: '20px'}} onSubmit = {handleSubmit}>
                        <Grid container spacing = {2}>
                            <Input name = "email" label = "UserName" handleChange = {handleChange} type = "email"/>
                            <Input name = "hashPassword" label = "Password" handleChange = {handleChange} type = {showPassword ? "text" : "password"} handleShowPassword = {handleShowPassword}/>
                            { isSignUp && <Input name = "confirmPassword" label = "Repeat Password" handleChange = {handleChange} type = "password"/>}
                        </Grid>
                        <div display={{margin: '20px',textDecoration: 'none',fontSize: '20px',fontFamily: 'Helvetica',color: '#000',cursor: 'pointer'}} onClick = {() => navigate('/forgotPassword')}>
                            Forgot password?
                        </div>
                        <Button type = "submit" fullWidth variant = "contained" color = "primary" sx = {{margin: "10px 0px 5px"}}>
                            {isSignUp ? 'Sign Up' : 'Sign In'}
                        </Button>
                        </form>
                </Paper>
            </Container>
            <Box marginLeft = '-4em' display='flex' justifyContent='center' alignItems='center'>
                <Button sx={{fontSize: '18px',fontFamily: 'Inter',fontWeight: 'normal',alignItems: 'center',display: 'flex',marginLeft: '200px'}} onClick = {switchMode}>
                    {isSignUp ? 'Already have an account ? Sign In' : "New member? Join now"}
                </Button>
            </Box> 
            </Container>
        </Grow>
    )
}

export default Auth;
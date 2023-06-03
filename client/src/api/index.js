import axios from 'axios';

const API = axios.create({baseURL: 'http://localhost:8080/api'});

API.interceptors.request.use((req) => {
    if(localStorage.getItem('profile')){
        req.headers.Authorization = `Bearer ${JSON.parse(localStorage.getItem('profile')).token}`
    }
    return req;
});

// Authentication
export const signin = (formData) => API.post('accounts/signin', formData);
export const signup = (formData) => API.post('accounts/signup', formData);

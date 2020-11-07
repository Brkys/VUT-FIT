<?php

/*
|--------------------------------------------------------------------------
| Web Routes
|--------------------------------------------------------------------------
|
| Here is where you can register web routes for your application. These
| routes are loaded by the RouteServiceProvider within a group which
| contains the "web" middleware group. Now create something great!
|
*/

Route::get('/', 'HomeController@home');

Auth::routes();

Route::get('/home', 'HomeController@index')->name('home');

Auth::routes();

Route::get('/test', 'TestController@test');

Route::get('/login', function(){
    return redirect('home')->with('openLogin', true);
});

Route::post('/login', 'loginController@login');

Route::get('/logout', 'loginController@logout');

Route::get('/register', function(){
    return redirect('home')->with('registerNotValid', true);
});

Route::post('/register', 'registerController@validateRegister');

Route::get('/news', 'NewsController@viewNews');

Route::get('/familias', 'FamiliasController@familias'); 

Route::get('/lands', 'LandsController@lands');

Route::get('/members', 'MembersController@members');

Route::get('/create-criminal', 'CreateCriminalController@create');

Route::get('/show-criminal', 'ShowCriminalController@show');

Route::get('/assign-criminal', 'AssignCriminalController@assign'); 

Route::get('/free-users', 'FreeUsersController@viewUsers');

Route::post('/free-users', 'FreeUsersController@invite');

Route::get('/admin', 'AdminController@admin');

Route::get('/account', 'loginController@account');

Route::post('/accept', 'loginController@accept');

Route::post('/decline', 'loginController@decline');

Route::get('/familia-editing', 'FamiliasController@editFamilia');

Route::post('/promote', 'FamiliasController@promote');

Route::post('/degrade', 'FamiliasController@degrade');

Route::post('/kick', 'FamiliasController@kick');

Route::post('/createNewFamilia', 'AdminController@createNewFamilia');

Route::post('/createCriminal', 'CreateCriminalController@createCriminal');

Route::post('/deleteFamilia', 'AdminController@deleteFamilia');

Route::post('/assign-criminal', 'AssignCriminalController@assignCriminal');  

Route::get('/no-permission', function() {
	session_start();
	return view('no-permission');
});

Route::get('/show-alliance', 'AlianceController@showAliances');

Route::post('/aliance-request', 'AlianceController@alianceRequest');

Route::post('/accept-aliance', 'AlianceController@acceptAliance');

Route::post('/decline-aliance', 'AlianceController@declineAliance');

Route::post('/change-account', 'loginController@changeAccount');
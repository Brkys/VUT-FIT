<?php

namespace App\Http\Controllers;

use Illuminate\Http\Request;
use App\newsModel;

class NewsController extends Controller
{
    public function viewNews()
    {
        session_start();
    	$news = newsModel::all()->sortByDesc('date');
    	return view('news')->with('news', $news);
        
    }

    public function addNews(Request $req)
    {
    	$news = new newsModel();
        $news->name = $req->input('date');
        $news->title = $req->input('title');
        $news->content = $req->input('content');
        $news->save();
        return redirect('news');
    }
}
